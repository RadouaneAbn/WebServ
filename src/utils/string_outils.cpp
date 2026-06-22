#include <utils.h>

std::string ft_itol(size_t n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
}

std::string ft_itoa(int n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
}

bool has_any(const std::string &s, const std::string &chars) {
    for (size_t i = 0; i < s.length(); ++i) {
        if (chars.find(s[i]) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool has_other(const std::string &s, std::string &chars) {
    for (size_t i = 0; i < s.length(); i++) {
        if (chars.find(s[i]) == std::string::npos) {
            return true;
        }
    }
    return false;
}

std::string trim(const std::string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    return s.substr(start, end - start + 1);
}

std::string to_lower(const std::string &s) {
    std::string result = s;
    for (size_t i = 0; i < s.length(); i++) {
        result[i] = std::tolower(s[i]);
    }
    return result;
}

std::string to_upper(const std::string &s) {
    std::string result = s;
    for (size_t i = 0; i < s.length(); i++) {
        result[i] = std::toupper(s[i]);
    }
    return result;
}

bool method_is_valid(const std::string& method) {
	return (method == "GET" || method == "POST" || method == "DELETE" || method == "HEAD");
}

std::string get_event_type(FDType t)
{
    switch (t)
    {
    case CLIENT_FD:
        return ("client :");
    case SERVER_FD:
        return ("server :");  
    case CGI_PIPE_FD:
        return ("CGI :");
    default:
        return ("unknown: ");
    }
}