#ifndef PARSER_HPP
#define PARSER_HPP

#include <Config.hpp>
#include <Lexer.hpp>

class Parser {
    Config *_config;
    size_t _pos;
    std::vector<Token> _tokens;

    void parseTokens();
    ServerBlock parseServerBlock();
    Location parseLocationBlock();
    void parseServerDirectives(ServerBlock &server_block);
    void parseLocationDirectives(Location &location);

    Token& advance( void );
    Token& peek( void );
    Token& ppeek( void );
    Token& expect(TokenType type);
    bool check(TokenType type);
public:
    Parser(std::string &filename);
    ~Parser();

    void printConfig(void) const;
};

/* Parser helper functions */
void save_listen_directive(ServerBlock &server, Token &directive, std::vector<std::string> &listens);
void save_root_directive(ServerBlock &server, Token &directive, std::vector<std::string> &roots);
void save_server_name_directive(ServerBlock &server, Token &directive, std::vector<std::string> &server_names);
void save_error_page_directive(ServerBlock &server, Token &directive, const std::vector<std::string> &args);
void save_client_max_body_size_directive(ServerBlock &server, Token &directive, std::vector<std::string> &sizes);

void save_root_directive(Location &location, Token &directive, std::vector<std::string> &roots);
void save_error_page_directive(Location &location, Token &directive, const std::vector<std::string> &args);
void save_client_max_body_size_directive(Location &location, Token &directive, std::vector<std::string> &sizes);
void save_autoindex_directive(Location &location, Token &directive, std::vector<std::string> &values);
void save_index_directive(Location &location, Token &directive, std::vector<std::string> &values);
void save_cgi_directive(Location &location, Token &directive, std::vector<std::string> &values);
void save_upload_path_directive(Location &location, Token &directive, std::vector<std::string> &values);
void save_upload_enabled_directive(Location &location, Token &directive, std::vector<std::string> &values);
void save_return_directive(Location &location, Token &directive, std::vector<std::string> &values);
void save_methods_directive(Location &location, Token &directive, std::vector<std::string> &values);
ssize_t parse_client_max_body_size(std::vector<std::string> &values, size_t line);



#endif // PARSER_HPP