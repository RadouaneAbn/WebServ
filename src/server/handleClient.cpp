#include <Server.hpp>

void Server::handleEvent(struct epoll_event &event)
{
    FdContext *context = static_cast<FdContext*>(event.data.ptr);
    if (*context & CLIENT_FD)
        acceptClient(context, event.events);
    else if (*context & SERVER_FD)
        handleClient(context, event.events);
    // else if (*context & CGI_PIPE_FD)
    //     handleCgi(context, event.events);
    else
        throw ServerException("Unknown event type");
}

void Server::acceptClient(FdContext *context, uint32_t events)
{
    int client_socket = accept(context->fd, NULL, NULL);
    if (client_socket < 0)
        throw std::runtime_error("accept() failed: " + std::string(strerror(errno)));

    Client *client = new Client(client_socket, context->fd, _addresses[context->fd]);
    create_epoll_event(_epoll_fd, client_socket, CLIENT_FD, EPOLLIN, client);

    std::map<int, ServerBlock*>::iterator it = _server_blocks.find(context->fd);
    if (it == _server_blocks.end())
        throw InternalServerErrorException("Coudn't find a server block for client");
    // client->request.setServerBlock(*it);
}

void Server::handleClient(FdContext *context, uint32_t events)
{
    if ((events & EPOLLERR) || (events & EPOLLHUP))
    {
        LOG_ERROR("Error in event of " + get_event_type(context->type) + ft_itoa(context->fd));
        destroyFdContext(context);
        return ;
    }
    else if (events & EPOLLIN)
    {

    }
    else if (events & EPOLLOUT)
    {

    }
    else
        throw InternalServerErrorException("Unknown client events");
}