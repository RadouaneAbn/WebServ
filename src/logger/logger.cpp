#include <Logger.hpp>

Logger::Logger() : _level(ERROR) {}

Logger::~Logger() {
    if (_file.is_open())
        _file.close();
}

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::setLevel(LogLevel level) {
    _level = level;
}

void Logger::setFile(const std::string& path) {
    _file.open(path.c_str(), std::ios::app);
}

void Logger::log(LogLevel level, const std::string& msg, const char* file, int line) {
    if (level < _level)
        return;

    std::ostringstream ss;
    ss << "[" << timestamp() << "] "
        << "[" << levelStr(level) << "] "
        << file << ":" << line << " — "
        << msg;

    std::string entry = ss.str();

    std::cerr << color(level) << entry << RESET_COLOR << std::endl;

    if (_file.is_open())
        _file << entry << std::endl;
}

void Logger::log2(LogLevel level, const std::string& msg) {
    if (level < _level)
        return;

    std::ostringstream ss;
    ss << msg;

    std::string entry = ss.str();

    std::cerr << color(level) << entry << RESET_COLOR << std::endl;

    if (_file.is_open())
        _file << entry << std::endl;
}

std::string Logger::timestamp() {
    time_t now = time(NULL);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return buf;
}

const char* Logger::levelStr(LogLevel l) {
    switch (l) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

const char* Logger::color(LogLevel l) {
    switch (l) {
        case SIMPLE:
            return "\033[33m"; // Yellow
        case DEBUG:
            return "\033[1;36m"; // Cyan
        case INFO:
            return "\033[1;32m"; // Green
        case WARNING:
            return "\033[1;33m"; // Yellow
        case ERROR:
            return "\033[1;31m"; // Red
        default:
            return "\033[0m";  // Reset
    }
}