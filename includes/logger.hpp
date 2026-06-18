#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

#define RESET_COLOR "\033[0m"

enum LogLevel
{
	DEBUG,
	INFO,
	WARNING,
	ERROR
};

class Logger
{

public:
	static Logger	&instance();
	void			setLevel(LogLevel level);
	void			setFile(const std::string &path);

	void			log(LogLevel level, const std::string &msg, const char *file, int line);

private:
	Logger();
	~Logger();
	LogLevel		_level;
	std::ofstream	_file;

	std::string		timestamp();
	const char		*levelStr(LogLevel l);
	const char		*color(LogLevel l);
};

// macros so file/line are captured automatically
#define LOG_DEBUG(msg) Logger::instance().log(DEBUG, msg, __FILE__, __LINE__)
#define LOG_INFO(msg) Logger::instance().log(INFO, msg, __FILE__, __LINE__)
#define LOG_WARN(msg) Logger::instance().log(WARNING, msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) Logger::instance().log(ERROR, msg, __FILE__, __LINE__)

#endif // LOGGER_HPP