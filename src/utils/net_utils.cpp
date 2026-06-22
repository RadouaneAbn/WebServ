#include <utils.h>
#include <Config.hpp>
#include <Server.hpp>

void setFdNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        throw std::runtime_error("fcntl() failed: " + std::string(strerror(errno)));

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl() failed: " + std::string(strerror(errno)));
}

struct sockaddr_in create_socket_info(ListenDirective &listen_directive)
{
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(listen_directive.port);
    if (inet_pton(AF_INET, listen_directive.ip.c_str(), &addr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid IP configuration: " + listen_directive.ip);
    }
    return addr;
}

FdContext *create_epoll_event(int epoll_fd, int fd, FDType type, uint32_t events, Client *client)
{
	struct epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));
	ev.events = events;
	FdContext* data = new FdContext(fd, type, client);
	if (!data) {
        if (client && type == CLIENT_FD)
            delete client;
        throw std::runtime_error("new failed: " + std::string(strerror(errno)));
	}
	ev.data.ptr = data;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        delete data;
		if (client && type == CLIENT_FD)
            delete(client);
        throw std::runtime_error("epoll_ctl failed: " + std::string(strerror(errno)));
	}
	return (data);
}

FdContext *update_epoll_event(int epoll_fd, uint32_t new_events, FdContext *fd_context)
{
    FdContext* data = fd_context;
	struct epoll_event event;
	event.events = new_events;
    event.data.ptr = data;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd_context->fd, &event) == -1)
		return (NULL);
	return (data);
}