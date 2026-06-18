#include <Lexer.hpp>

Token::Token(TokenType type, std::string value, size_t line) : type(type), value(value), line(line) {}

Lexer::Lexer() : _line(0) {}

Lexer::~Lexer() {}

void Lexer::tokenizeFile(std::string &filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
    
}

void Lexer::TokenizeLine( std::string line )
{
    // Implementation of tokenization logic goes here
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