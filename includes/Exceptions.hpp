#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>

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

class LexerException : public ConfigException
{
public:
    LexerException(const std::string &msg = "Lexer error") : ConfigException(msg) {};
    ~LexerException() throw() {};

};

class ParserException : public ConfigException
{
public:
    ParserException(const std::string &msg = "Parser error") : ConfigException(msg) {};
    ~ParserException() throw() {};
};

#endif // EXCEPTIONS_HPP