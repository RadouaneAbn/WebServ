#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <set>
#include <vector>
#include <sys/epoll.h>
#include <Config.hpp>
#include <utils.h>
#include <cerrno>
#include <Logger.hpp>
#include <Client.hpp>

#define SOCKET_TIMEOUT 5000

extern volatile sig_atomic_t g_server_running;

class Client;

struct FdContext
{
	FdContext(int fd, FDType type, Client *client = NULL);
	FdContext(FdContext &old_obj);
	FdContext operator =(FdContext &old_obj);
	~FdContext(void);

	int      fd;
	FDType   type;
	Client  *client;

	bool operator&(FDType t) const;
};

class Server
{
public:
	typedef std::vector<ListenDirective>::iterator ListenIter;
	typedef std::vector<ServerBlock>::iterator ServerIter;
	typedef std::map<int, FdContext*>::iterator FdContextIter;

	typedef std::vector<ListenDirective>::const_iterator ListenConstIter;
	typedef std::vector<ServerBlock>::const_iterator ServerConstIter;
	typedef std::map<int, FdContext*>::const_iterator FdContextConstIter;

	Server(Config *config);
	~Server( void );

	void setupSockets( void );
	void initEpoll( void );
	void initServer( void );
	void startServer( void );
	FdContext *getFdContext(int fd);


private:
	Config							*_config;
	int								_epoll_fd;
	std::map<int, FdContext*>		_fd_contexts;
	std::vector<int>				_listen_fds;
	std::map<int, ListenDirective*>	_addresses;
	std::map<int, ServerBlock*>		_server_blocks;

	int createListenSocket(ListenDirective &listen_directive);
	void handleEvent(FdContext *context);
	void acceptClient(FdContext *context);
	// void handleClient(FdContext *context);
	// void handleCgi(FdContext *context);
};

#endif // SERVER_HPP