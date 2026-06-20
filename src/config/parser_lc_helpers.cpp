#include <Parser.hpp>
#include <Lexer.hpp>
#include <Exceptions.hpp>
#include <utils.h>
#include <cstdlib>

void save_root_directive(Location &location, Token &directive, std::vector<std::string> &values) {
    if (values.size() != 1)
        throw ParserException("Multiple root directives are not allowed" + ft_itol(directive.line));
    location.root = values[0];
}

void save_error_page_directive(Location &location, Token &directive, const std::vector<std::string> &values)
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

        location.error_pages[error_code] = error_path;
    }
}

void save_client_max_body_size_directive(Location &location, Token &directive, std::vector<std::string> &values)
{
    location.client_max_body_size = parse_client_max_body_size(values, directive.line);
}

void save_autoindex_directive(Location &location, Token &directive, std::vector<std::string> &values)
{
    if (values.size() != 1)
        throw ParserException("Autoindex directive requires exactly one value at line: " + ft_itol(directive.line));
    if (values[0] == "on")
        location.autoindex = true;
    else if (values[0] == "off")
        location.autoindex = false;
    else
        throw ParserException("Invalid value for autoindex directive: '" + values[0] + "' at line: " + ft_itol(directive.line));
}

void save_index_directive(Location &location, Token &directive, std::vector<std::string> &values)
{
    if (values.empty())
        throw ParserException("Index directive requires at least one value at line: " + ft_itol(directive.line));
    location.index = values[0];
}

void save_cgi_directive(Location &location, Token &directive, std::vector<std::string> &values)
{
    if (values.size() != 2)
        throw ParserException("CGI directive requires exactly two values at line: " + ft_itol(directive.line));
    location.cgi[values[0]] = values[1];
    location.is_cgi = true;
}

void save_upload_path_directive(Location &location, Token &directive, std::vector<std::string> &values)
{
    if (values.size() != 1)
        throw ParserException("Upload path directive requires exactly one value at line: " + ft_itol(directive.line));
    location.upload_path = values[0];
}

void save_upload_enabled_directive(Location &location, Token &directive, std::vector<std::string> &values)
{
    if (values.size() != 1)
        throw ParserException("Upload enabled directive requires exactly one value at line: " + ft_itol(directive.line));
    if (values[0] == "on")
        location.upload_enabled = true;
    else if (values[0] == "off")
        location.upload_enabled = false;
    else
        throw ParserException("Invalid value for upload enabled directive: '" + values[0] + "' at line: " + ft_itol(directive.line));
}

void save_return_directive(Location &location, Token &directive, std::vector<std::string> &values)
{
    if (values.size() != 2)
        throw ParserException("Return directive requires exactly two values at line: " + ft_itol(directive.line));
    
    std::string code_str = values[0];
    for (size_t j = 0; j < code_str.length(); j++)
    {
        if (!std::isdigit(static_cast<unsigned char>(code_str[j])))
        throw ParserException("Invalid HTTP redirect code '" + code_str + "' at line: " + ft_itol(directive.line));
    }
    if (code_str.size() != 3)
        throw ParserException("Redirect code must be a 3-digit number at line: " + ft_itol(directive.line));
    int redirect_code = std::atoi(code_str.c_str());
    location.redirect.first = redirect_code;
    location.redirect.second = values[1];
}

void save_methods_directive(Location &location, Token &directive, std::vector<std::string> &values)
{
    location.upload_enabled = false;
    if (values.empty())
        throw ParserException("Methods directive requires at least one value at line: " + ft_itol(directive.line));
    location.methods.insert(location.methods.end(), values.begin(), values.end());
    for (int i = 0; i < static_cast<int>(location.methods.size()); ++i)
    {
        std::string method = location.methods[i];
        if (method == "POST")
            location.upload_enabled = true;
    }
}
