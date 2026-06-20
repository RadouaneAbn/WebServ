#include <iomanip>
#include <sstream>
#include <Parser.hpp>
#include <colors.h>
#include <utils.h>
#include <iostream>

static void printLine(const std::string& label, const std::string& value, int indent)
{
    std::cout << std::string(indent, ' ')
               << GRAY << std::left << std::setw(22) << label << RESET
               << value << "\n";
}

static void printErrorPages(const std::map<int, std::string>& pages, int indent)
{
    if (pages.empty())
        return;
    std::cout << std::string(indent, ' ') << GRAY << "error_pages:" << RESET << "\n";
    for (std::map<int, std::string>::const_iterator it = pages.begin(); it != pages.end(); ++it)
        std::cout << std::string(indent + 2, ' ') << YELLOW << it->first << RESET
                   << " -> " << it->second << "\n";
}

static void printCgi(const std::map<std::string, std::string>& cgi, int indent)
{
    if (cgi.empty())
        return;
    std::cout << std::string(indent, ' ') << GRAY << "cgi:" << RESET << "\n";
    for (std::map<std::string, std::string>::const_iterator it = cgi.begin(); it != cgi.end(); ++it)
        std::cout << std::string(indent + 2, ' ') << MAGENTA << "." << it->first << RESET
                   << " -> " << it->second << "\n";
}

static void printVector(const std::string& label, const std::vector<std::string>& vec, int indent)
{
    if (vec.empty())
        return;
    std::ostringstream ss;
    for (size_t i = 0; i < vec.size(); i++)
    {
        ss << vec[i];
        if (i + 1 < vec.size())
            ss << ", ";
    }
    printLine(label, ss.str(), indent);
}

static void printLocation(const Location& loc, int indent)
{
    std::cout << std::string(indent, ' ') << BOLD << BLUE << "└─ location " << RESET
               << GREEN << loc.path << RESET << "\n";

    int in = indent + 4;
    printLine("root:",            loc.root, in);
    printLine("index:",           loc.index, in);
    printLine("autoindex:",       loc.autoindex ? "on" : "off", in);
    printVector("methods:",       loc.methods, in);
    printLine("client_max_body:", loc.client_max_body_size == -1
                                       ? "(inherit)"
                                       : ft_itol(loc.client_max_body_size) + " bytes", in);
    printLine("upload_enabled:",  loc.upload_enabled ? "true" : "false", in);
    if (loc.upload_enabled)
        printLine("upload_path:", loc.upload_path, in);
    if (loc.redirect.first != -1)
        printLine("return:", ft_itol(loc.redirect.first) + " -> " + loc.redirect.second, in);
    printCgi(loc.cgi, in);
    printErrorPages(loc.error_pages, in);
    std::cout << "\n";
}

static void printServerBlock(const ServerBlock& sb, size_t index)
{
    std::cout << BOLD << CYAN << "╭─ server[" << index << "] " 
               << std::string(50, '-') << RESET << "\n";

    int in = 3;

    std::cout << std::string(in, ' ') << GRAY << "listen:" << RESET << "\n";
    for (size_t i = 0; i < sb.listen_directives.size(); i++)
        std::cout << std::string(in + 2, ' ') << YELLOW
                   << sb.listen_directives[i].ip << ":" << sb.listen_directives[i].port
                   << RESET << "\n";

    printVector("server_name:", sb.server_names, in);
    printLine("root:", sb.root, in);
    printLine("client_max_body:", sb.client_max_body_size == -1
                                       ? "(default)"
                                       : ft_itol(sb.client_max_body_size) + " bytes", in);
    printErrorPages(sb.error_pages, in);

    std::cout << "\n";
    if (!sb.locations.empty())
    {
        std::cout << std::string(in, ' ') << GRAY << "locations (" << sb.locations.size() << "):" << RESET << "\n\n";
        for (size_t i = 0; i < sb.locations.size(); i++)
            printLocation(sb.locations[i], in + 2);
    }

    std::cout << BOLD << CYAN << "╰─" << std::string(58, '-') << RESET << "\n\n";
}

void Parser::printConfig(void) const
{
    std::cout << "\n" << BOLD << "CONFIG SUMMARY — "
               << _config->_server_blocks.size() << " server block(s)" << RESET << "\n\n";

    for (size_t i = 0; i < _config->_server_blocks.size(); i++)
        printServerBlock(_config->_server_blocks[i], i);
}