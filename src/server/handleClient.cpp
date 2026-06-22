#include <Server.hpp>

void Server::handleEvent(FdContext *context)
{
    if (*context & CLIENT_FD)
        acceptClient(context);
    // else if (*context & SERVER_FD)
    //     handleClient(context);
    // else if (*context & CGI_PIPE_FD)
    //     handleCgi(context);
    else
        throw ServerException("Unknown event type");
}

void Server::acceptClient(FdContext *context)
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