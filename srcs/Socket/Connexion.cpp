#include <Socket.hpp>
#include <Ressource.hpp>
#include <Utils.hpp>

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
																request(t_http_message()),
																is_header_parsed(false),
																is_request_line_parsed(false),
																is_body_parsed(false),
																header_readed_size(0),
																body_readed_size(0),
																dechunker(request.body),
																route(NULL),
																ressource(NULL),
																response(http_header_formatter(200, 20) + "Placeholder response"),
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

	if (recv_size == -1)
		return IOEvent(FAIL, this, "Error happened while reading socket");
	if (!recv_size)
		return closed();
	if (is_body_parsed)
		return setError("allready received an entire request", 400);
	raw_request.append(buffer, recv_size);
	if (!is_header_parsed)
		return readHeader();
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

t_http_message &Connexion::getRequest() { return request; }

void Connexion::append_response(std::string message) { response.append(message); }
void Connexion::append_response(std::string message, size_t n) { response.append(message, n); }


//
//		Underlying operations
//


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
	size_t	pos = 0;
	std::vector<std::string> lines;

	while (pos != std::string::npos && !is_header_parsed)
	{
		if (header_readed_size >= MAX_HEADER_SIZE)
			return setError("header exceeds max header size", 413);
		pos = raw_request.find(CRLF);
		lines.push_back(raw_request.substr(0, pos));
		raw_request.erase(0, pos + 2);
		header_readed_size += pos;
		if (!pos)
			is_header_parsed = true;
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
	std::string	methodString = splitted_line[0];
	request.request_line.methodVerbose = methodString;
	request.request_line.method = methodToEnum(methodString);
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
		is_body_parsed = true;
	}
	else {
		if (!request.header_fields["Content-Length"].empty()){
			std::stringstream	content_length_converter;
			content_length_converter << request.header_fields["Content-Length"];
			if (!(content_length_converter >> request.content_length))
				return setError("Content-Length header field is not correct", 400);
		}
	}

	if (request.header_fields["Host"].empty())
		return setError("missing Host header field", 400);

	// SETTING UP ROUTE AND RESSOURCE
	route = router.getRoute(netAddr, request);
	if (!route)
		return setError("internal error - route not found", 500);
	response = http_header_formatter(200, route->getAttributes().location.length()) + route->getAttributes().location;
	// route->printAttributes();
	route->createRessource(request, this);
	if (request.content_length > route->getMaxBodySize())
		return setError("Content-Length header field is bigger than the maximum body size allowed for this route", 413);
	// route->handle();

	// if we already have a body in raw_request
	if (!raw_request.empty())
		return readBody();
	return IOEvent();
}

IOEvent	Connexion::readBody() {
	// if chunked request
	if (request.header_fields["Transfer-Encoding"] != "" && request.header_fields["Transfer-Encoding"] != "identity") {
		IOEvent	io_resp = dechunker(raw_request);
		if (io_resp.result == FAIL)
			return setError(io_resp.log, io_resp.http_error);
		else if (io_resp.result == SUCCESS)
		{
			if (epoll_util(EPOLL_CTL_MOD, c_socket, this, EPOLLIN | EPOLLOUT))
				return setError("unable to set EPOLL_CTL_MOD", 500);
			is_body_parsed = true;
		}
	}
	else {
		request.body.append(raw_request, 0, request.content_length - request.body.size());
		raw_request.clear();
		if (request.body.size() == request.content_length) {
			if (epoll_util(EPOLL_CTL_MOD, c_socket, this, EPOLLOUT))
				return setError("unable to set EPOLL_CTL_MOD", 500);
			is_body_parsed = true;
		}
	}
	return IOEvent();
}
