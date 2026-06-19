#ifndef SERVER_HPP
#define SERVER_HPP

#include <webserv.h>

struct Location {
	bool						autoindex;
	std::string					index;
	std::string					path;
	std::string					root;
	std::vector<std::string>	methods;
	ssize_t						client_max_body_size;
	std::string					upload_dir;
};

struct ServerConfig {
	std::map<int, std::string>	listen;
	std::string					root;
	std::vector<Location>		locations;
};

class Server {
	Server();

public:
	Server *getInstance( void );
};

#endif // SERVER_HPP