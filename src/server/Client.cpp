#include <Client.hpp>

Client::Client(int fd, int listen_fd, ListenDirective *listen_directive) :
    _fd(fd), _listen_fd(listen_fd), _listen_directive(listen_directive)
{
    (void) _fd;
    (void) _listen_fd;
    (void) _listen_directive;
}

Client::~Client(void)
{
}