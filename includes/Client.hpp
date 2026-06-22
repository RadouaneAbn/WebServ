#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <Config.hpp>

class Client
{
    int _fd;
    int _listen_fd;
    std::string res;
    ListenDirective *_listen_directive;
public:
    Client(int fd, int listen_fd, ListenDirective *listen_directive);
    ~Client();
};

#endif // CLIENT_HPP