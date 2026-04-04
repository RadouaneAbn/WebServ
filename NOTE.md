
```
Client -> Request -> Header.
```

## Client class

```c++

class Client {
	int socket_fd;
	std::string _raw_bytes;
	Request _request;
	...
}

```

## REQUEST PARSING:

```c++
/**
  * Request: 
  */

enum RequestState {
	READ_START_LINE,	// Parse first line: "METHOD SP TARGET SP HTTP/VERSION\r\n"
	READ_HEADERS,		// Parse headers until empty line: "\r\n\r\n"
	READ_PLAIN_BODY,	// Read body bytes based on Content-Length
	READ_CHUNK_BODY,	// Read chunked body until the final zero-size chunk is parsed
	FINISHED			// Request fully parsed and ready for routing/handling
};

enum RequestMethod {
    GET,
    POST,
    DELETE
};

class IRequest {
	RequestState _state;		// ...
    RequestMethod _method;	// ...
    std::string _buffer;			// ...
	std::string _path;				// ...

	Headers *_headers;				// ...
	std::string _raw_bytes;

public:
	Request();
	~Request();

	void read_request( void );

	void extract_first_line( void );
	void extract_headers( void );
	void extract_body( void );

}
```

### Headers parsing:

*All header keys are case-insensitive, so convert keys to lowercase* **before saving**.

```c++

/**
	* `Headers` parsing is triggered when the main parser is in `READ_HEADERS`
	* and detects the header terminator "\r\n\r\n".
	* It then splits the request at "\r\n\r\n" and parses the first part.
  */
class IHeaders {
	// the headers and cookies map
	std::map <std::string, std::string> _headers;
	std::map <std::string, std::string> _cookies;

	void _parse_line( const std::string &line ); // delimiter is "\r\n"
	void _extract_key_value( const std::string &line, char del ); // extract key: value for both headers and cookies (':' for header, '=' for cookie)
	void _parse_cookies( const std::string &line ); // delimiter is ';'
	std::string _trim( const std::string &str ); // this will turn to a function to be used in other classes

public:
	// constructor that takes the raw headers string
	Headers( std::string &headers_str );
	~Headers();

	// verify that Host exists in the request headers
	bool validate_headers( void );

	// getters for all header or cookies
	std::string build_headers_response( void ) const; // build a header string to append to the response

	// getter for a header or cookie value
	const std::string &get_header_value( const std::string &key );
	const std::string &get_cookie_value( const std::string &key );

	/**
	  * Exceptions
	  * Map each exception to an HTTP status code in the request handling layer.
	  */
	class WrongHeaderFormat: public exception {
		virtual const char *what( void ) const throw(); // status: 400
	};

	class HeaderKeyNotFound: public exception {
		virtual const char *what( void ) const throw();
	};

	class CookieKeyNotFound: public exception {
		virtual const char *what( void ) const throw();
	};
};

std::ostream &operator<<(std::ostream &o, Headers &obj)
{
	o << "";
	return (o);
}

```

> Note: If a message contains both `Transfer-Encoding` and `Content-Length`, **`Transfer-Encoding`** overrides `Content-Length`.

```c++

/**
	* RequestState: enum to determine the current parsing step
	* of an incoming HTTP request.
	*
	* Normal flow:
	* READ_START_LINE -> READ_HEADERS -> READ_PAIN_BODY / READ_CHNK_BODY -> FINISHED
	*
	* Notes:
	* - A request without body can go directly from READ_HEADERS to FINISHED.
	* - Use READ_PAIN_BODY when body size is known from Content-Length.
	* - Use READ_CHNK_BODY when Transfer-Encoding is chunked.
	* - The parser must keep unread bytes between states (same client buffer).
	* - Move to next state only when current section is fully parsed.
  */

enum RequestState {
	READ_START_LINE,	// Parse first line: "METHOD SP TARGET SP HTTP/VERSION\r\n"
	READ_HEADERS,		// Parse headers until empty line: "\r\n\r\n"
	READ_PLAIN_BODY,	// Read body bytes based on Content-Length
	READ_CHUNK_BODY,	// Read chunked body until the final zero-size chunk is parsed
	FINISHED			// Request fully parsed and ready for routing/handling
};

```

#### `RequestState` details

- `READ_START_LINE`
	- Wait until one full line ending with `\r\n` is available.
	- Extract and validate: method, path/target, and HTTP version.
	- On success -> `READ_HEADERS`.


- `READ_HEADERS`
	- Parse header lines one by one until the empty line (`\r\n\r\n`).
	- Normalize/store headers in a map (case-insensitive keys).
	- Decide next state:
		- If `Transfer-Encoding: chunked` -> `READ_CHUNK_BODY`
		- Else if body is expected (e.g. `Content-Length`) -> `READ_PLAIN_BODY`
		- Otherwise -> `FINISHED`


- `READ_PLAIN_BODY`
	- Read exactly the expected body payload.
	- Body length is determined by `Content-Length`.
	- On complete body -> `FINISHED`.

- `READ_CHUNK_BODY`
	- Parse and accumulate HTTP chunks.
	- Continue until the terminating zero-size chunk is received.
	- On complete body -> `FINISHED`.

- `FINISHED`
	- Parsing is done.
	- Request object is complete and safe to pass to routing/response logic.