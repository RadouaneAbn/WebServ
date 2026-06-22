#ifndef HEADER_HPP
#define HEADER_HPP

#include <iostream>
#include <string>
#include <map>
#include <exception>

#include <HttpExceptions.hpp>
#include <colors.h>
#include <utils.h>

class Header {
private:
    std::map<std::string, std::string> _headers;
    std::map<std::string, std::string> _cookies;

    void _parser( const std::string &s );
    void _cookies_parser( const std::string &s );
    void _header_pair_parser( const std::string &s, char del);
    void _cookie_pair_parser( const std::string &s, char del);

public:
    Header( void );
    ~Header( void );

    void parseHeaders( std::string &headers_string );
    const std::string& getHeader( const std::string &key ) const;
    const std::string& getCookie( const std::string &key ) const;
    void setHeader( const std::string &key, const std::string &value );
    bool hasHeader( const std::string &key ) const;
    bool hasCookie( const std::string &key ) const;
    std::map<std::string, std::string> &getHeaders();

	class HeaderNotFound: public std::exception {
		virtual const char *what( void ) const throw();
	};

	class CookieNotFound: public std::exception {
		virtual const char *what( void ) const throw();
	};
};

#endif // HEADER_HPP