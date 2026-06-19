#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <webserv.h>

enum TokenType {
	TOKEN_KEYWORD,		// "server", "listen", "root", "location"
	TOKEN_VALUE,		// "8080", "./site", "/upload"
	TOKEN_LBRACE,		// "{"
	TOKEN_RBRACE,		// "}"
	TOKEN_SEMICOLON,	// ";"
	TOKEN_EOF			// End of file
};

struct Token {
	Token(TokenType type, std::string value, size_t line);

	TokenType	type;
	std::string	value;
	size_t		line;	// Crucial for printing clear configuration error lines!
};

class Lexer {
private:
	std::string			_filename;
	size_t				_line;
	std::vector<Token>	_tokens;
	std::ifstream		_file;

	void		TokenizeLine( std::string &line );
	void		addToken( TokenType type, std::string value, size_t line );
	bool		isSpecialChar( char c );

public:
	Lexer(std::string &filename);
	~Lexer();

	void		init( void );
	const		std::vector<Token> getTokens( void ) const;
	void		printTokens( void );
};

#endif // TOKENIZER_HPP