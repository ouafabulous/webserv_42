#include <Socket.hpp>
#include <Ressource.hpp>

inline bool ends_with(std::string const &value, std::string const &ending)
{
	if (ending.size() > value.size())
		return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

// CONSTRUCTORS & DESTRUCTOR

Connexion::Connexion(const t_network_address netAddr,
					 const t_fd socket, const Router &router) : c_socket(socket),
																netAddr(netAddr),
																router(router),
																is_header_parsed(false),
																is_request_line_parsed(false),
																header_readed_size(0),
																body_readed_size(0),
																route(NULL),
																ressource(NULL),
																response_start(false)
{
}
Connexion::~Connexion()
{
	Logger::debug << c_socket << " was delete" << std::endl;
	if (epoll_util(EPOLL_CTL_DEL, c_socket, this, EPOLLIN))
		throw std::runtime_error("epoll_del failed");
	close(c_socket);
}

// IOEvent

IOEvent Connexion::read()
{
	ssize_t recv_size = recv(c_socket, buffer, BUFFER_SIZE, MSG_DONTWAIT);

	Logger::debug << "read\n";
	if (recv_size == -1)
		return IOEvent(FAIL, this, "Error happened while reading socket");
	// 	// request_header.append(buffer);
	if (!recv_size)
		return IOEvent(FAIL, this, "Client closed the connexion");
	buffer[recv_size] = 0;
	if (!is_header_parsed)
	{
		header_readed_size += recv_size;
		return readHeader();
	}
	body_readed_size += recv_size;
	return readBody();
}
IOEvent Connexion::write()
{
	if (response.empty())
		return IOEvent();
	Logger::debug << "write to conn" << std::endl;
	if (send(c_socket, response.c_str(), response.length(), MSG_DONTWAIT) == -1)
		return IOEvent(FAIL, this, "unable to write to the client socket");
	return IOEvent(SUCCESS, this, "successfuly send response");
}
IOEvent Connexion::closed() { return IOEvent(FAIL, this, "client closed the connexion"); }

// Underlying operations

IOEvent Connexion::setError(std::string log, uint http_error)
{
	std::string body;

	if (response_start)
		return IOEvent(FAIL, this, log);
	response.clear();
	Logger::warning << http_error << " " << log << std::endl;
	if (route)
		body = route->getError(http_error);
	else
		body = Errors::getDefaultError(http_error);
	response.append(http_header_formatter(http_error, body.length()));
	response.append(body);
	if (ressource)
	{
		delete ressource;
		ressource = NULL;
	}
	if (epoll_util(EPOLL_CTL_MOD, c_socket, this, EPOLLOUT))
		return IOEvent(FAIL, this, "unable to epoll_ctl_mod");
	return IOEvent();
}

IOEvent Connexion::readHeader()
{
	std::string::iterator line_start;
	std::string current_line;
	std::vector<std::string> lines;

	raw_request.append(buffer);
	if (header_readed_size >= MAX_HEADER_SIZE)
		return setError("header exceeds max header size", 413);

	line_start = raw_request.begin();
	for (std::string::iterator it = raw_request.begin() + 1; it != raw_request.end() && !is_header_parsed; it++)
	{
		if (*(it - 1) == '\r' && *it == '\n')
		{
			if (line_start == it - 1)
			{ // Detect end of header (/r/n/r/n)
				lines.push_back("");
				is_header_parsed = true;
			}
			else {
				current_line.assign(line_start, it - 1);
				lines.push_back(current_line);
			}
			line_start = it + 1;
		}
	}
	raw_request.erase(raw_request.begin(), line_start);
	if (!raw_request.empty() && is_header_parsed) { // if one part of the body is in the raw_request
		header_readed_size -= raw_request.size();
		body_readed_size += raw_request.size();
	}
	if (lines.size() > 0)
		return parseHeader(lines);
	return IOEvent();
}
IOEvent Connexion::parseHeader(std::vector<std::string> &lines)
{
	std::vector<std::string>::iterator it = lines.begin();
	size_t pos = 0;
	std::string key;
	std::string value;

	if (!is_request_line_parsed && parseRequestLine(*it++)) // parsing of request line
		return setError("request_line is invalid", 400);
	for (; it != lines.end(); it++)
	{ // parsing of all header fields
		if (it->length() == 0)
			return executeRoute();
		if ((pos = (*it).find(":")) == std::string::npos)
			return setError("header fields not RFC conform", 400);
		key = it->substr(0, pos);
		value = it->substr(it->find_first_not_of(' ', pos + 1));
		pos = value.find_last_not_of(' ');
		if (pos != std::string::npos)
			value.erase(pos + 1);
		if (key.empty() || value.empty() || key.find(' ') != std::string::npos)
			return setError("header fields not RFC conform", 400);
		if (request.header_fields.find(key) != request.header_fields.end())
			request.header_fields[key].append(",");
		request.header_fields[key].append(value);
	}
	return IOEvent();
}

bool Connexion::parseRequestLine(std::string &raw_line)
{
	std::vector<std::string> splitted_line;
	size_t pos = 0;
	while (pos != std::string::npos)
	{
		pos = raw_line.find(" ");
		splitted_line.push_back(raw_line.substr(0, pos));
		raw_line.erase(0, pos + 1);
	}
	if (splitted_line.size() != 3 || splitted_line[2] != HTTP_VERSION)
		return NOK;
	request.request_line.method = splitted_line[0];
	request.request_line.path = splitted_line[1];
	request.request_line.http_version = splitted_line[2];
	is_request_line_parsed = true;
	return OK;
}

IOEvent Connexion::executeRoute()
{
	// if we are not waiting for a body
	if (request.request_line.method == "GET" ||
		(request.header_fields["Transfer-Encoding"].empty() && request.header_fields["Content-Length"].empty()))
	{
		if (body_readed_size > 0)
		{
			Logger::debug << body_readed_size << std::endl;
			return setError("request contains body but GET Method, no Transfer-Encoding or Content-Length given", 400);
		}
		if (epoll_util(EPOLL_CTL_MOD, c_socket, this, EPOLLOUT))
			return setError("unable to set EPOLL_CTL_MOD", 500);
	}
	else {
		if (!request.header_fields["Content-Length"].empty()){
			std::stringstream	content_length_converter;
			content_length_converter << request.header_fields["Content-Length"];
			if (!(content_length_converter >> request.content_length))
				return setError("Content-Length header field is not correct", 400);
		}
		if (epoll_util(EPOLL_CTL_MOD, c_socket, this, EPOLLIN | EPOLLOUT))
			return setError("unable to set EPOLL_CTL_MOD", 500);
	}

	// SETTING UP ROUTE AND RESSOURCE
	route = router.getRoute(netAddr, request);
	if (!route)
		return setError("internal error", 500);
	if (request.content_length > route->getMaxBodySize())
		return setError("Content-Length header field is bigger than the maximum body size allowed for this route", 413);
	// response = "HTTP/1.1 404 OK\nContent-Type:text/html\nContent-Length: 49\n\n<html><body><h1>File not found</h1></body></html>";
	// route->handle();


	// if we already have a body in raw_request
	if (!raw_request.empty())
		return readBody();
	return IOEvent();
}

IOEvent	Connexion::readBody() {
	Logger::debug << "read body" << std::endl;

	// if chunked request
	// if (request.header_fields["Transfer-Encoding"] != "" && request.header_fields["Transfer-Encoding"] != "identity")
	// 	return parseChunkedBody();
	if (body_readed_size == request.content_length && epoll_util(EPOLL_CTL_MOD, c_socket, this, EPOLLOUT))
		return setError("unable to set EPOLL_CTL_MOD", 500);
	else if (body_readed_size > request.content_length)
		return setError("body to large", 413);
	request.body.append(raw_request);
	raw_request.clear();
	return IOEvent();
}

t_http_message &Connexion::getRequest() { return request; }

void Connexion::append_response(std::string message, size_t n)
{
	if (n == 0)
		response.append(message);
	else
		response.append(message, n);
}
