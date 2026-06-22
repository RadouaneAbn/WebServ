═══════════════════════════════════════════════════════
  MULTIPLEXING CORE — pseudocode for the event loop
═══════════════════════════════════════════════════════

GLOBAL STATE (members of Server)
    epoll_fd            : int
    listen_fds           : vector<int>
    fd_context           : map<int, FdContext*>      // every fd epoll knows about
    clients               : map<int, Client*>          // fd -> client object
    fd_to_servers        : map<int, vector<ServerBlock*>>  // listen_fd -> eligible server blocks


─────────────────────────────────────────
start() — the main loop
─────────────────────────────────────────

FUNCTION start():
    events = array of struct epoll_event[MAX_EVENTS]

    WHILE NOT shutdown_requested:
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT_MS)

        IF nfds < 0:
            IF errno == EINTR:
                CONTINUE                      // interrupted by signal, just retry
            LOG_ERROR("epoll_wait failed")
            BREAK

        FOR i in 0..nfds:
            TRY:
                dispatch(events[i])
            CATCH HttpException as e:
                handle_http_error(events[i], e)
            CATCH std::exception as e:
                LOG_ERROR(e.what())

        check_timeouts()                      // runs every iteration, cheap if nothing expired


─────────────────────────────────────────
dispatch() — route event to the right handler
─────────────────────────────────────────

FUNCTION dispatch(event):
    ctx = (FdContext*) event.data.ptr

    IF ctx is NULL:
        LOG_WARN("event with null context, ignoring")
        RETURN

    SWITCH ctx.type:
        CASE SERVER_FD:
            accept_client(ctx.fd)

        CASE CLIENT_FD:
            handle_client_event(ctx, event.events)

        CASE CGI_PIPE_FD:
            handle_cgi_event(ctx, event.events)


─────────────────────────────────────────
accept_client() — new connection
─────────────────────────────────────────

FUNCTION accept_client(listen_fd):
    LOOP:                                      // drain all pending connections this round
        client_fd = accept(listen_fd)

        IF client_fd < 0:
            IF errno == EAGAIN OR errno == EWOULDBLOCK:
                BREAK                          // no more pending connections
            LOG_WARN("accept failed")
            BREAK

        set_non_blocking(client_fd)

        client = new Client(client_fd, listen_fd)
        clients[client_fd] = client

        ctx = new FdContext(client_fd, CLIENT_FD, client)
        fd_context[client_fd] = ctx

        register_fd(client_fd, EPOLLIN, ctx)

        LOG_INFO("client connected fd=" + client_fd)


─────────────────────────────────────────
handle_client_event() — request lifecycle
─────────────────────────────────────────

FUNCTION handle_client_event(ctx, events):
    client = ctx.client

    IF events & EPOLLHUP OR events & EPOLLERR:
        close_client(client.fd)
        RETURN

    IF events & EPOLLIN:
        bytes = read(client.fd, buffer, BUFFER_SIZE)

        IF bytes == 0:
            close_client(client.fd)            // client closed connection
            RETURN
        IF bytes < 0:
            IF errno == EAGAIN: RETURN          // nothing to read right now
            close_client(client.fd)
            RETURN

        client.request.append(buffer, bytes)    // feeds the chunked/plain body parser

        IF client.request.is_complete():
            route_info = router.route(client.request, find_server_block(client))

            IF route_info.action == EXECUTE_CGI:
                cgi_handler.start(*this, client, route_info)   // spawns child, registers pipe fd
            ELSE:
                response = build_response(route_info)
                client.response = response
                switch_to_write_mode(client.fd)

    IF events & EPOLLOUT:
        flush_client_write(client)


─────────────────────────────────────────
flush_client_write() — non-blocking send
─────────────────────────────────────────

FUNCTION flush_client_write(client):
    remaining = client.response.length() - client.bytes_sent

    sent = send(client.fd,
                client.response.c_str() + client.bytes_sent,
                remaining)

    IF sent < 0:
        IF errno == EAGAIN: RETURN              // socket buffer full, wait for next EPOLLOUT
        close_client(client.fd)
        RETURN

    client.bytes_sent += sent

    IF client.bytes_sent == client.response.length():
        IF client.keep_alive:
            reset_client_for_next_request(client)
            switch_to_read_mode(client.fd)
        ELSE:
            close_client(client.fd)


─────────────────────────────────────────
handle_cgi_event() — CGI pipe I/O
─────────────────────────────────────────

FUNCTION handle_cgi_event(ctx, events):
    client    = ctx.client
    cgi_state = client.cgi_state

    cgi_state.last_activity = now()             // any activity resets the timeout

    IF events & EPOLLERR:
        cleanup_cgi(cgi_state, killed=true)
        respond_with_error(client, 502)
        RETURN

    IF events & EPOLLIN:
        bytes = read(cgi_state.res_r_fd, buffer, BUFFER_SIZE)
        IF bytes > 0:
            cgi_state.output.append(buffer, bytes)
        // bytes == 0 handled by EPOLLHUP below in most epoll implementations

    IF events & EPOLLHUP:
        finalize_cgi(client, cgi_state)          // parse response, build client.response, switch to write mode


─────────────────────────────────────────
check_timeouts() — runs once per loop iteration
─────────────────────────────────────────

FUNCTION check_timeouts():
    now = current_time()
    expired = []                                 // collect first, mutate after (avoid iterator invalidation)

    FOR EACH client IN clients:
        IF client.cgi_state EXISTS:
            IF now - client.cgi_state.last_activity > CGI_TIMEOUT:
                expired.append(client)
        ELSE:
            IF client.request.last_activity_too_old(now):
                expired.append(client)

    FOR EACH client IN expired:
        IF client.cgi_state EXISTS:
            cleanup_cgi(client.cgi_state, killed=true)
        respond_with_error(client, 504)


─────────────────────────────────────────
close_client() — full teardown
─────────────────────────────────────────

FUNCTION close_client(fd):
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL)
    close(fd)

    IF clients[fd].cgi_state EXISTS:
        cleanup_cgi(clients[fd].cgi_state, killed=true)

    delete fd_context[fd]
    delete clients[fd]
    fd_context.erase(fd)
    clients.erase(fd)


─────────────────────────────────────────
register_fd() / switch_to_*_mode() — epoll helpers
─────────────────────────────────────────

FUNCTION register_fd(fd, events, ctx):
    ev.events   = events
    ev.data.ptr = ctx
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev)

FUNCTION switch_to_write_mode(fd):
    ev.events   = EPOLLOUT
    ev.data.ptr = fd_context[fd]
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev)

FUNCTION switch_to_read_mode(fd):
    ev.events   = EPOLLIN
    ev.data.ptr = fd_context[fd]
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev)