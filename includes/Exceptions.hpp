#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <webserv.h>

class ConfigException : public std::exception
{
    std::string _message;
public:
	ConfigException(std::string message) : _message(message) {};
	~ConfigException() throw() {};

	const char *what() const throw()
	{
		return _message.c_str();
	}
};

class LexerException : public std::exception
{
public:
    LexerException();
    ~LexerException() throw() {};

};

#endif // EXCEPTIONS_HPP