#include <Client.hpp>

Client::Client(int fd, int listen_fd, ListenDirective *listen_directive) :
    _fd(fd), _listen_fd(listen_fd), _listen_directive(listen_directive), _cgi_state(NULL)
{
    (void) _fd;
    (void) _listen_fd;
    (void) _listen_directive;
}

Client::~Client(void)
{
}

void Client::appendToBuffer(char *s, size_t size)
{
    s[size];
    _request.append_request(s, size);
}

