#include <Parser.hpp>
#include <Lexer.hpp>
#include <Exceptions.hpp>
#include <utils.h>
#include <cstdlib>

void save_listen_directive(ServerBlock &server, Token &directive, std::vector<std::string> &values) {
    if (values.size() > 1)
        throw ParserException("Multiple listen directives are not allowed" + ft_itol(directive.line));

    std::string ip;
    int port;
    std::string listen = values[0];
    size_t colon_pos = listen.find(':');
    if (colon_pos != std::string::npos)
    {
        ip = listen.substr(0, colon_pos);
        port = std::atoi(listen.substr(colon_pos + 1).c_str());
    }
    else
    {
        ip = "0.0.0.0";
        port = std::atoi(listen.c_str());
    }
    if (port < 1 || port > 65535)
        throw ParserException("Invalid port number in listen directive: " + listen + ft_itol(directive.line));
    server.listen_directives.push_back(ListenDirective(ip, port));
}

void save_root_directive(ServerBlock &server, Token &directive, std::vector<std::string> &values) {
    if (values.size() != 1)
        throw ParserException("Multiple root directives are not allowed" + ft_itol(directive.line));
    server.root = values[0];
}

void save_server_name_directive(ServerBlock &server, Token &directive, std::vector<std::string> &values) {
    if (values.empty())
        throw ParserException("Server name directive requires at least one value" + ft_itol(directive.line));
    server.server_names.insert(server.server_names.end(), values.begin(), values.end());
}

void save_error_page_directive(ServerBlock &server, Token &directive, const std::vector<std::string> &values)
{
    if (values.size() < 2)
        throw ParserException("Error page directive requires at least two values at line: " + ft_itol(directive.line));

    std::string error_path = values[values.size() - 1];

    for (size_t i = 0; i < values.size() - 1; i++)
    {
        std::string code_str = values[i];

        for (size_t j = 0; j < code_str.length(); j++)
        {
            if (!std::isdigit(static_cast<unsigned char>(code_str[j])))
                throw ParserException("Invalid HTTP error code '" + code_str + "' at line: " + ft_itol(directive.line));
        }

        int error_code = std::atoi(code_str.c_str());
        if (error_code < 300 || error_code > 599)
            throw ParserException("HTTP error code '" + code_str + "' out of range at line: " + ft_itol(directive.line));

        server.error_pages[error_code] = error_path;
    }
}

void save_client_max_body_size_directive(ServerBlock &server, Token &directive, std::vector<std::string> &values)
{
    server.client_max_body_size = parse_client_max_body_size(values, directive.line);
}
