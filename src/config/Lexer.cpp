#include <Lexer.hpp>

Token::Token(TokenType type, std::string value, size_t line) : type(type), value(value), line(line) {}

Lexer::Lexer( std::string &filename ) : _line(0) {
	_filename = filename;
	_file.open(filename.c_str());
	if (!_file.good())
		throw LexerException("Could not open configuration file: " + filename);
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
				addToken(TOKEN_LBRACE, "}", _line);
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

const std::vector<Token> Lexer::getTokens( void ) const
{
	return _tokens;
}

void Lexer::printTokens( void )
{
	// size_t diff = 0;
	for (std::vector<Token>::const_iterator it = _tokens.begin(); it < _tokens.end(); it++)
	{
		std::string value = it->value;
		if (it->type == TOKEN_KEYWORD || it->type == TOKEN_VALUE)
			value += " ";
		else if (it->type == TOKEN_LBRACE || it->type == TOKEN_RBRACE || it->type == TOKEN_SEMICOLON)
			value += " ";
		std::cout << value;
	}
}