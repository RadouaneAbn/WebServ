#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <cstdlib>

struct Location {
	Location() : autoindex(false), client_max_body_size(-1), upload_enabled(false) {}
	bool								autoindex;
	std::string							index;
	std::string							path;
	std::string							root;
	std::vector<std::string>			methods;
	ssize_t								client_max_body_size;
	std::map<int, std::string>			error_pages;
	std::map<std::string, std::string>	cgi;
	bool								is_cgi;
	std::string							upload_path;
	bool								upload_enabled;
	std::pair<int, std::string>			redirect;
};

struct ListenDirective {
	ListenDirective(std::string ip_addr, int port_number) : ip(ip_addr), port(port_number) {}
	std::string					ip;
	int							port;
};

struct ServerBlock {
	ServerBlock() : client_max_body_size(-1) {}
	std::vector<ListenDirective>	listen_directives;
	std::string						root;
	std::vector<Location>			locations;
	std::vector<std::string>		server_names;
	std::map<int, std::string>		error_pages;
	ssize_t							client_max_body_size;
};

class Config {
public:
	std::vector<ServerBlock>	_server_blocks;
};

#endif // CONFIG_HPP