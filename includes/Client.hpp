#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <Config.hpp>
#include <Request.hpp>

struct CgiState {
    int pid;
};

class Client
{
    int _fd;
    int _listen_fd;
    std::string res;
    ListenDirective *_listen_directive;
    Request         _request;
    std::string     _buffer;
    ListenDirective *_listen_directive;
    int             _listen_fd;

    CgiState     *_cgi_state;
public:
    void appendToBuffer(char *s, size_t size);
    Client(int fd, int listen_fd, ListenDirective *listen_directive);
    ~Client();
};

#endif // CLIENT_HPP