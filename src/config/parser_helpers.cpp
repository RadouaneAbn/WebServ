#include <Parser.hpp>
#include <Lexer.hpp>
#include <Exceptions.hpp>
#include <utils.h>
#include <climits>
#include <cstdlib>

ssize_t parse_client_max_body_size(std::vector<std::string> &values, size_t line)
{
    if (values.size() != 1)
        throw ParserException("Client max body size directive requires exactly one value at line: " + ft_itol(line));

    std::string size_str = values[0];
    if (size_str.empty())
        throw ParserException("Client max body size cannot be empty at line: " + ft_itol(line));

    ssize_t multiplier = 1;
    bool has_suffix = false;
    char last_char = size_str[size_str.length() - 1];

    if (last_char == 'K' || last_char == 'k') { multiplier = 1024; has_suffix = true; }
    else if (last_char == 'M' || last_char == 'm') { multiplier = 1024 * 1024; has_suffix = true; }
    else if (last_char == 'G' || last_char == 'g') { multiplier = 1024 * 1024 * 1024; has_suffix = true; }
    else if (!std::isdigit(static_cast<unsigned char>(last_char)))
        throw ParserException("Invalid size suffix '" + std::string(1, last_char) + "' at line: " + ft_itol(line));

    std::string number_part = has_suffix ? size_str.substr(0, size_str.length() - 1) : size_str;

    if (number_part.empty())
        throw ParserException("Client max body size requires a numeric value at line: " + ft_itol(line));

    for (size_t i = 0; i < number_part.length(); i++)
    {
        if (!std::isdigit(static_cast<unsigned char>(number_part[i])))
            throw ParserException("Invalid character in client max body size: '" + std::string(1, number_part[i]) + "' at line: " + ft_itol(line));
    }

    errno = 0;
    long long parsed = std::strtoll(number_part.c_str(), NULL, 10);
    if (errno == ERANGE)
        throw ParserException("Client max body size value out of range at line: " + ft_itol(line));

    if (parsed != 0 && multiplier > 0 && parsed > (LLONG_MAX / multiplier))
        throw ParserException("Client max body size value too large at line: " + ft_itol(line));
    return (static_cast<ssize_t>(parsed) * multiplier);
}