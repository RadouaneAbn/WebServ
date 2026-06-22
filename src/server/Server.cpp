#include <Server.hpp>

volatile sig_atomic_t g_server_running = true;

Server::Server(Config *config)
{
    _config = config;
}

void Server::initServer(void)
{
    setupSockets();
    initEpoll();
}

void Server::startServer(void)
{
    struct epoll_event events[10];

    while (g_server_running)
    {
        int events_count = epoll_wait(_epoll_fd, events, 10, SOCKET_TIMEOUT);
        if (events_count < 0)
        {
            if (errno == EINTR)
                continue;
            std::runtime_error("epoll_wait() failed: " + std::string(strerror(errno)));
        }

        for (int i = 0; i < events_count; i++)
        {
            FdContext *context = static_cast<FdContext*>(events[i].data.ptr);
            try
            {
                handleEvent(context);
            }
            catch (const HttpException &e)
            {
                LOG_ERROR(e.what());
                // kick(context);
            }
            catch (const std::exception &e)
            {
                LOG_ERROR(e.what());
                // kick(context);
            }
        }
    }
}

Server::~Server( void )
{
    for (FdContextIter it = _fd_contexts.begin(); it != _fd_contexts.end(); it++)
    {
        int current_fd = it->first;
        FdContext *data = it->second;
        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, current_fd, NULL);
        close(current_fd);
        delete data;
    }
    _fd_contexts.clear();
    close(_epoll_fd);
}

FdContext *Server::getFdContext(int fd)
{
    return (_fd_contexts[fd]);
}