#include <colors.h>
#include <Lexer.hpp>
#include <Exceptions.hpp>
#include <string>
#include <iostream>
#include <iomanip>

Token::Token(TokenType type, std::string value, size_t line) : type(type), value(value), line(line) {}

Lexer::Lexer( std::string &filename ) : _line(0) {
	_filename = filename;
	_file.open(filename.c_str());
	if (!_file.good())
		throw LexerException("Could not open configuration file: " + filename);
	init();
}


Lexer::~Lexer() {
	if (_file.is_open())
		_file.close();
}

void Lexer::init( void )
{
	std::string line;
	while (std::getline(_file, line))
	{
		_line++;
		TokenizeLine(line);
	}
	addToken(TOKEN_EOF, "", _line);
}

void Lexer::TokenizeLine( std::string &line )
{
	bool key_found = false;
	std::string word = "";
	// std::cout << "Line[" << _line << "]: " << line << std::endl;
	for (size_t i = 0; i < line.size(); i++)
	{
		char c = line[i];

		if (isspace(c)) {
			if (!word.empty())
			{
				if (!key_found)
					addToken(TOKEN_KEYWORD, word, _line);
				else
					addToken(TOKEN_VALUE, word, _line);
				key_found = true;
				word.clear();
			}
			// std::cout << '.';
			continue;
		}
		else if (isSpecialChar(c))
		{
			if (!word.empty())
			{
				if (!key_found)
					addToken(TOKEN_KEYWORD, word, _line);
				else
					addToken(TOKEN_VALUE, word, _line);
			}
			if (c == '{')
				addToken(TOKEN_LBRACE, "{", _line);
			else if (c == '}')
				addToken(TOKEN_RBRACE, "}", _line);
			else if (c == ';')
				addToken(TOKEN_SEMICOLON, ";", _line);
		}
		else if (c == '#')
		{
			if (!word.empty())
			{
				if (!key_found)
					addToken(TOKEN_KEYWORD, word, _line);
				else
					addToken(TOKEN_VALUE, word, _line);
			}
			break;
		}
		else
			word += c;
	}
}

void Lexer::addToken( TokenType type, std::string value, size_t line )
{
	_tokens.push_back(Token(type, value, line));
}

bool Lexer::isSpecialChar( char c )
{
	return (c == '{' || c == '}' || c == ';');
}

const std::vector<Token>& Lexer::getTokens( void ) const
{
	return _tokens;
}

void Lexer::printTokens(void)
{
    const char *type_names[] = {
        "KEYWORD", "VALUE", "LBRACE", "RBRACE", "SEMICOLON", "EOF"
    };
    const char *colors[] = {
        "\033[36m", "\033[33m", "\033[35m", "\033[35m", "\033[90m", "\033[31m"
    };

    int indent = 0;

    for (std::vector<Token>::const_iterator it = _tokens.begin(); it->type != TOKEN_EOF; it++)
    {
        if (it->type == TOKEN_RBRACE)
            indent--;

        std::cout << "[" << std::setw(3) << it->line << "] "
                   << std::string(indent * 2, ' ')
                   << colors[it->type] << std::setw(10) << std::left << type_names[it->type]
                   << RESET << it->value << std::endl;

        if (it->type == TOKEN_LBRACE)
            indent++;
    }
}