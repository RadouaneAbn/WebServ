#include <Parser.hpp>
#include <Lexer.hpp>
#include <Exceptions.hpp>
#include <utils.h>

Parser::Parser(std::string &filename) : _pos(0)
{
    Lexer lexer(filename);
    _config = new Config();
    if (!_config)
        throw ParserException("Memory allocation failed for Config object");
    _tokens = lexer.getTokens();
    if (_tokens.empty())
        throw ParserException("Configuration file is empty: " + filename);
    parseTokens();
    printConfig();
}

Parser::~Parser() {}

void Parser::parseTokens(void)
{
    while (peek().type != TOKEN_EOF)
    {
        const Token &token = peek();

        if (token.type == TOKEN_KEYWORD && token.value == "server")
            _config->_server_blocks.push_back(parseServerBlock());
        else
            throw ParserException("Unexpected token '" + token.value + "' at line " + ft_itol(token.line) + ". Expected 'server' block.");
    }
    if (_config->_server_blocks.empty())
        throw ParserException("No server blocks found in configuration file.");
}

ServerBlock Parser::parseServerBlock(void)
{
    expect(TOKEN_KEYWORD); // "server"
    expect(TOKEN_LBRACE);  // "{"

    ServerBlock server_block;

    while (!check(TOKEN_RBRACE))
    {
        if (check(TOKEN_EOF))
            throw ParserException("unexpected end of file, missing '}' at line " + ft_itol(peek().line));
        
        if (peek().value == "location")
            server_block.locations.push_back(parseLocationBlock());
        else if (peek().type == TOKEN_KEYWORD)
            parseServerDirectives(server_block);
        else
            throw ParserException("Unexpected token '" + peek().value + "' at line " + ft_itol(peek().line));
    }

    expect(TOKEN_RBRACE);
    return server_block;
}

Location Parser::parseLocationBlock( void )
{
    expect(TOKEN_KEYWORD);  // "location"
    std::string path = expect(TOKEN_VALUE).value;
    expect(TOKEN_LBRACE);   // "{"

    Location location;
    location.path = path;

    while (!check(TOKEN_RBRACE))
    {
        if (check(TOKEN_EOF))
            "unexpected end of file, missing '}' at line " + ft_itol(peek().line);
        
        parseLocationDirectives(location);
    }

    expect(TOKEN_RBRACE);
    return (location);
}

void Parser::parseServerDirectives(ServerBlock &server_block)
{
    Token directive = expect(TOKEN_KEYWORD);
    std::string name = directive.value;

    std::vector<std::string> values;
    while (check(TOKEN_VALUE))
        values.push_back(advance().value);
    
    expect(TOKEN_SEMICOLON);

    if (values.empty())
        throw ParserException("directive '" + name + "' requires a value" + ft_itol(directive.line));
    
    if (name == "listen")
        save_listen_directive(server_block, directive, values);
    else if (name == "root")
        save_root_directive(server_block, directive, values);
    else if (name == "server_name")
        save_server_name_directive(server_block, directive, values);
    else if (name == "error_page")
        save_error_page_directive(server_block, directive, values);
    else if (name == "client_max_body_size")
        save_client_max_body_size_directive(server_block, directive, values);
    else
        throw ParserException("Unknown directive '" + name + "' at line " + ft_itol(directive.line));
}

void Parser::parseLocationDirectives(Location &location)
{
    Token directive = expect(TOKEN_KEYWORD);
    std::string name = directive.value;

    std::vector<std::string> values;
    while (check(TOKEN_VALUE))
        values.push_back(advance().value);
    
    expect(TOKEN_SEMICOLON);

    if (values.empty())
        throw ParserException("directive '" + name + "' requires a value" + ft_itol(directive.line));
    
    if (name == "root")
        save_root_directive(location, directive, values);
    else if (name == "error_page")
        save_error_page_directive(location, directive, values);
    else if (name == "client_max_body_size")
        save_client_max_body_size_directive(location, directive, values);
    else if (name == "index")
        save_index_directive(location, directive, values);
    else if (name == "methods")
        save_methods_directive(location, directive, values);
    else if (name == "cgi")
        save_cgi_directive(location, directive, values);
    else if (name == "autoindex")
        save_autoindex_directive(location, directive, values);
    else if (name == "upload_path")
        save_upload_path_directive(location, directive, values);
    else if (name == "upload_enabled")
        save_upload_enabled_directive(location, directive, values);
    else if (name == "return")
        save_return_directive(location, directive, values);
    else
        throw ParserException("Unknown directive '" + name + "' at line " + ft_itol(directive.line));
}

Token &Parser::advance(void)
{
    Token &token = _tokens[_pos];
    if (_pos < _tokens.size() - 1)
        _pos++;
    return token;
}

Token &Parser::peek(void)
{
    return _tokens[_pos];
}

std::string tokenTypeToString(TokenType type)
{
    switch (type)
    {
    case TOKEN_KEYWORD:
        return "KEYWORD";
    case TOKEN_VALUE:
        return "VALUE";
    case TOKEN_LBRACE:
        return "{";
    case TOKEN_RBRACE:
        return "}";
    case TOKEN_SEMICOLON:
        return ";";
    case TOKEN_EOF:
        return "EOF";
    default:
        return "UNKNOWN";
    }
}

Token &Parser::expect(TokenType type)
{
    if (check(type))
        return advance();
    throw ParserException("Expected token type " + tokenTypeToString(type) + " but got " + tokenTypeToString(peek().type) + " at line " + ft_itol(peek().line));
}

bool Parser::check(TokenType type)
{
    return peek().type == type;
}
