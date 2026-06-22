#include <Server.hpp>
#include <Logger.hpp>

int Server::createListenSocket(ListenDirective &listen_directive)
{
    struct sockaddr_in addr = create_socket_info(listen_directive);
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(sock);
        throw std::runtime_error("setsockopt() failed: " + std::string(strerror(errno)));
    }

    if (bind(sock, (struct sockaddr *)(&addr), sizeof(addr)) < 0)
    {
        close(sock);
        throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));
    }

    if (listen(sock, SOMAXCONN) < 0)
    {
        close(sock);
        throw std::runtime_error("listen() failed: " + std::string(strerror(errno)));
    }

    setFdNonBlocking(sock);

    LOG_INFO("Listenning to: " + listen_directive.ip + ":" + ft_itoa(listen_directive.port));

    return (sock);
}

void Server::setupSockets( void )
{
    std::vector<ServerBlock> &server_blocks = _config->_server_blocks;
    std::set<ListenDirective> seen;

    for (ServerIter sv_it = server_blocks.begin(); sv_it < server_blocks.end(); sv_it++)
    {
        std::vector<ListenDirective> &listen_directives = sv_it->listen_directives;
        for (ListenIter ld_it = listen_directives.begin(); ld_it != listen_directives.end(); ld_it++)
        {
            if (seen.find(*ld_it) != seen.end())
                continue;

            seen.insert(*ld_it);

            int fd = createListenSocket(*ld_it);
            _listen_fds.push_back(fd);
            // ListenDirective ld = ListenDirective(it->ip, it->port);
            _addresses[fd] = &(*ld_it);
            _server_blocks[fd] = &(*sv_it);
        }
    }

    if (_listen_fds.empty())
        throw std::runtime_error("No listen directives are given");
}

void Server::initEpoll( void )
{
    _epoll_fd = epoll_create1(0);
    if (_epoll_fd < 0)
        throw std::runtime_error("epoll_create1() failed: " + std::string(strerror(errno)));
    
    for (size_t i = 0; i < _listen_fds.size(); i++)
    {
        int fd = _listen_fds[i];
        _fd_contexts[fd] = create_epoll_event(_epoll_fd, fd, SERVER_FD, EPOLLIN, NULL);
    }
}
