#ifndef SERVER_HPP
#define SERVER_HPP

#include <webserv.h>

class Server {
    Server();

public:
    Server *getInstance( void );
};

#endif // SERVER_HPP