#include <Socket.hpp>

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

// CONSTRUCTORS & DESTRUCTOR

Connexion::Connexion(const t_network_address netAddr,
	const t_fd socket, const Router &router) :	c_socket(socket),
												netAddr(netAddr),
												router(router),
												header_end(false),
												route(NULL),
												ressource(NULL)
{}
Connexion::~Connexion() {
	if (epoll_util(EPOLL_CTL_DEL, c_socket, this, EPOLLIN))
		throw std::runtime_error("epoll_del failed");
	close(c_socket);
}


// IOEvent

IOEvent	Connexion::read() {
	ssize_t	recv_size;

	Logger::debug << "read from conn" << std::endl;
	if ((recv_size = recv(c_socket, buffer, BUFFER_SIZE, MSG_DONTWAIT)) < 1)
		return IOEvent(FAIL, this, "Connexion socket closed");
		// request_header.append(buffer);
	buffer[recv_size] = 0;
	if (!header_end)
		return readHeader();
	// else
	// 	return readBody();
	// if (ends_with(request_header, "\n\r")) {
	// }
	return IOEvent();
}
IOEvent	Connexion::write() {
	int	flags = fcntl(c_socket, F_GETFL, O_NONBLOCK);
	if (flags == -1)
		return IOEvent(FAIL, this, "connexion socket closed");
	std::string text = "HTTP/1.1 200 OK\nContent-Type:text/plain\nContent-Length: 12\n\nHello World!";
	Logger::debug << "write to conn" << std::endl;
	if (send(c_socket, text.c_str(), text.length(), MSG_DONTWAIT) < 1)
		return IOEvent(FAIL, this, "unable to write to the client socket");
	return IOEvent(SUCCESS, this, "successfuly send response");
}
IOEvent	Connexion::closed() { return IOEvent(FAIL, this, "client closed the connexion"); }

void	Connexion::writeError(uint http_error) {
	std::stringstream	response;
	std::string			body;

	if (route)
		body = route->getError(http_error);
	else
		body = Errors::getDefaultError(http_error);
	response << http_header_formatter(http_error, body.length());
	response << body;
	send(c_socket, response.str().c_str(), response.str().length(), MSG_DONTWAIT);
}

// Underlying operations

IOEvent	Connexion::readHeader() {
	request_header.append(buffer);
	size_t	header_end_pos = request_header.find(CRLF);
	Logger::debug << "read header" << std::endl;
	if (request_header.length() >= MAX_HEADER_SIZE)
		return IOEvent(FAIL, this, "header exceeds max header size", 413);
	if (header_end_pos != std::string::npos) {
		Logger::debug << "End of header detected" << std::endl;
		request.body.assign(request_header.begin() + header_end_pos + 4, request_header.end());
		header_end = true;
		route = router.getRoute(netAddr, request);
		return parseHeader();
	}
	return IOEvent();
}
IOEvent	Connexion::parseHeader() {
	return IOEvent(FAIL, this, "file not found", 404);
}
// bool	Connexion::readBody() {}
