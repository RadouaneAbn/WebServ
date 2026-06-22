#include <Request.hpp>
#include <unistd.h>
#include <cstdlib>
#include <sys/socket.h>
#include <algorithm>

#define BUFFER_SIZE 4096

size_t Request::n_file = 0;

Request::Request(void)
{
	_last_activity_time = std::time(NULL);
	_state = READ_START_LINE;
	_method = "";
	_path = "";
	_pos = 0;
	_body_size = 0;
	_content_length = 0;
	_read_bytes = 0;
	_body_is_set = false;
	_parse[READ_START_LINE] = &Request::extract_first_line;
	_parse[READ_HEADERS] = &Request::extract_headers;
	_parse[READ_PLAIN_BODY] = &Request::extract_plain_body;
	_parse[READ_CHUNK_BODY] = &Request::extract_chunked_body;
}

Request::~Request(void)
{
	if (_outfile.is_open())
	{
		_outfile.flush();
		_outfile.close();
	}
	unlink(_filename.c_str());
}

bool Request::is_finished(void)
{
	return (_state == FINISHED);
}

const std::string &Request::get_path(void) const
{
	return _path;
}

const std::string &Request::get_method(void) const
{
	return _method;
}

std::string &Request::get_filename(void)
{
	return _filename;
}

ssize_t Request::read_to_file(const char *s, ssize_t size)
{
	if (!_outfile.is_open())
		throw InternalServerErrorException("Temporary file for request body is not open");
	if (_state == READ_PLAIN_BODY && _body_size + static_cast<size_t>(size) > _content_length) {
		size = _content_length - _body_size;
	}
	_outfile.write(s, size);
	_body_size += size;
	return size;
}

void Request::append_request(const char *s, ssize_t size)
{
	reset_timeout();
	_buffer.append(s, size);
	_parser();
}

void Request::_parser(void)
{
	if (_state == FINISHED)
		return;
	if ((this->*_parse[_state])())
		_parser();
}

bool Request::is_timedout( time_t time ) const
{
	return (_last_activity_time + CONNECTION_TIMEOUT_SEC < time);
}

void Request::reset_timeout( void )
{
	_last_activity_time = std::time(NULL);
}

bool Request::extract_first_line(void)
{
	size_t sp_pos = _buffer.find("\r\n");
	if (sp_pos == std::string::npos)
		return false;
	std::string first_line = _buffer.substr(0, sp_pos);
	size_t first_sp, second_sp;
	first_sp = first_line.find_first_of(' ');
	second_sp = first_line.find_last_of(' ');
	if (first_line.find(' ', first_sp + 1) != second_sp)
		throw BadRequestException("Invalid request line");
	_method = first_line.substr(0, first_sp);
	_path = first_line.substr(first_sp + 1, second_sp - first_sp - 1);
	_http_version = first_line.substr(second_sp + 1);
	if (!method_is_valid(_method))
		throw NotImplementedException("Unsupported HTTP method: " + _method);
	if (_path.empty() || (_http_version != "HTTP/1.1" && _http_version != "HTTP/1.0"))
		throw BadRequestException("Invalid request line");
	_buffer = _buffer.substr(sp_pos + 2);
	_pos = 0;
	_state = READ_HEADERS;
	return true;
}

const std::string &Request::get_http_version(void) const
{
	return _http_version;
}

void Request::start_save_to_file(void)
{
	char buf[17];
	std::ifstream file("/dev/urandom");
	if (!file.is_open())
		throw InternalServerErrorException("Failed to open /dev/urandom for generating temporary filename");
	file.read(buf, 16);
	buf[16] = '\0';
	file.close();
	_filename = "/tmp/request_" + ft_itoa(++n_file) + ".tmp";
	_outfile.open(_filename.c_str(), std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);
	if (!_outfile.is_open())
		throw InternalServerErrorException("Failed to create temporary file for request body");
}

void Request::extract_headers_t_e( void )
{
	std::string value = _headers.getHeader("transfer-encoding");
	to_lower(value);
	if (value == "chunked")
		_state = READ_CHUNK_BODY;
	else
		throw NotImplementedException("Transfer-Encoding not supported");
	start_save_to_file();
}

void Request::extract_headers_c_l( void )
{
	std::string charset = "0123456789";
	std::string value = _headers.getHeader("content-length");
	if (value.empty() || has_other(value, charset))
		throw BadRequestException("Invalid Content-Length header");
	long n = std::strtol(value.c_str(), NULL, 10);
	_content_length = n;
	if (_content_length > static_cast<size_t>(_request_info.server_block->client_max_body_size))
		throw PayloadTooLargeException("Body size exceeds server limit: " + ft_itoa(_request_info.server_block->client_max_body_size));
	if (n == 0)
		_state = FINISHED;
	else
		_state = READ_PLAIN_BODY;
	start_save_to_file();
}

bool Request::extract_headers(void)
{
	size_t sp_pos = _buffer.find("\r\n\r\n");
	if (sp_pos == std::string::npos)
		return (false);
	std::string header_str = _buffer.substr(_pos, sp_pos - _pos);
	_headers.parseHeaders(header_str);
	_pos = sp_pos + 4;
	if (_headers.hasHeader("transfer-encoding"))
		extract_headers_t_e();
	else if (_headers.hasHeader("content-length"))
		extract_headers_c_l();
	else if (_method == "POST")
		throw LengthRequiredException("POST request missing Content-Length header");
	else
		_state = FINISHED;
	_buffer = _buffer.substr(_pos);
	_pos = 0;

	return (true);
}

RequestState Request::get_state(void) const
{
	return _state;
}

bool Request::extract_plain_body(void)
{
    size_t bytes_to_read = _content_length - _body_size;
    size_t available = std::min(_buffer.size(), bytes_to_read);
	read_to_file(_buffer.c_str(), available);
	_buffer.clear();
	if (_body_size < _content_length)
		return (false);
	_state = FINISHED;
	if (_body_size != _content_length)
		throw BadRequestException("Body size does not match Content-Length");
	return (true);
}

void Request::close_outfile(void)
{
	if (_outfile.is_open())
		_outfile.close();
}
bool Request::extract_chunked_body(void)
{
	std::string sep1("\r\n", 2);
	std::string sep2("\r\n\r\n", 4);
	std::string dechunked_body;

	while (true)
	{
		if (_pos > _buffer.size())
		{
			_pos = 0;
			_buffer.clear();
			return (false);
		}
		size_t chunk_start = _pos;
		size_t n = _buffer.find(sep1, _pos);

		if (n == std::string::npos)
		{
			if (_pos != 0)
			{
				_buffer = _buffer.substr(_pos);
				_pos = 0;
			}
			return (false);
		}

		std::string hex = _buffer.substr(_pos, n - _pos);

		if (hex.find_first_not_of("0123456789aAbBcCdDeEfF") != std::string::npos)
			throw BadRequestException("Invalid chunk size: [" + hex + "]");

		_pos += 2 + hex.size();
		unsigned int chunk_size = std::strtol(hex.c_str(), NULL, 16);

		if (chunk_size == 0)
		{
			if (_buffer.find(sep1, _pos) == std::string::npos)
			{
				_buffer = _buffer.substr(chunk_start);
				_pos = 0;
				return (false);
			}
			_state = FINISHED;
			_headers.setHeader("content-length", ft_itoa(_body_size));
			size_t final_crlf = _buffer.find(sep1, _pos);
			if (final_crlf != std::string::npos)
				_pos = final_crlf + 2;
			break;
		}

		if (_pos + chunk_size <= _buffer.size())
		{
			size_t after_data = _pos + chunk_size;
			if (after_data + 2 > _buffer.size())
			{
				_buffer = _buffer.substr(chunk_start);
				_pos = 0;
				return (false);
			}

			std::string chunk = _buffer.substr(_pos, chunk_size);
			read_to_file(chunk.c_str(), chunk.size());
			if (_body_size > static_cast<size_t>(_request_info.server_block->client_max_body_size))
				throw PayloadTooLargeException("Body size exceeds server limit: " + ft_itoa(_request_info.server_block->client_max_body_size));
			_pos += chunk_size + 2;
		}
		else
		{
			_buffer = _buffer.substr(chunk_start);
			_pos = 0;
			return (false);
		}
	}
	_buffer = _buffer.substr(_pos);
	_pos = 0;
	return (true);
}

size_t Request::get_content_length(void) const
{
	return _content_length;
}

const std::string &Request::getHeader(const std::string &name) const
{
	return _headers.getHeader(name);
}

std::string &Request::get_body(void)
{
	std::ifstream file;
	if (_body_is_set == false)
	{
		file.open(_filename.c_str(), std::ios::binary | std::ios::in);
		char buf[BUFFER_SIZE];
		while (file.read(buf, sizeof(buf)))
		{
			_body.append(buf, file.gcount());
		}
		if (file.gcount() > 0)
		{
			_body.append(buf, file.gcount());
		}
		_body_is_set = true;
	}
	return _body;
}

std::map<std::string, std::string> &Request::getHeaders()
{
	return _headers.getHeaders();
}

void Request::setServerBlock(ServerBlock *server_block)
{
	if (server_block == NULL)
		throw InternalServerErrorException("Server block is NULL");
	_request_info.server_block = server_block;
}

ServerBlock *Request::getServerBlock() const
{
	return _request_info.server_block;
}

std::string &Request::get_query_string(void)
{
	return _query_string;
}

void Request::set_request_info(const std::string &root, const std::string &filepath)
{
	_request_info.root = root;
	_request_info.filepath = filepath;
}

std::string Request::get_filepath(void) const
{
	return _request_info.filepath;
}

std::string Request::get_root(void) const
{
	return _request_info.root;
}

void Request::set_filepath(const std::string &filepath)
{
	_request_info.filepath = filepath;
}

const std::string &Request::getCookie(const std::string &name) const
{
	return _headers.getCookie(name);
}

void Request::setCookie(const std::string &name, const std::string &value)
{
	_headers.setHeader("Set-Cookie", name + "=" + value);
}