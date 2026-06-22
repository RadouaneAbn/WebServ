#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <iostream>

// #include <Server.hpp>
#include <Exceptions.hpp>
#include <HttpExceptions.hpp>
// #include <Config.hpp>

struct FdContext;
struct ListenDirective;
class Client;

enum FDType
{
    SERVER_FD,
    CLIENT_FD,
    CGI_PIPE_FD
};

std::string ft_itol(size_t n);
std::string ft_itoa(int n);
void setFdNonBlocking(int fd);
struct sockaddr_in create_socket_info(ListenDirective &listen_directive);
FdContext *create_epoll_event(int epoll_fd, int fd, FDType type, uint32_t events, Client *client);
void handle_signal(int signum);

#endif // UTILS_H