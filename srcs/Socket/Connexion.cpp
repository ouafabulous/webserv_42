#include <Socket.hpp>

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

Connexion::Connexion(const t_network_address netAddr,
	const t_fd socket, const Router &router) :	c_socket(socket),
												netAddr(netAddr),
												router(router)
{}

Connexion::~Connexion() {
	epoll_util(EPOLL_CTL_DEL, c_socket, this, EPOLLIN);
	close(c_socket);
}
IOEvent	Connexion::read() {
	char	buffer[BUFFER_SIZE];
	ssize_t	recv_size = -1;

	Logger::debug << "read from conn" << std::endl;
	if ((recv_size = recv(c_socket, buffer, BUFFER_SIZE, MSG_DONTWAIT)) > 0) {
		buffer[recv_size -1] = 0;
		request_header.append(buffer);
	}
	else
		return IOEvent(FAIL, this, "Connexion socket closed");
	if (ends_with(request_header, "\n\r")) {
		epoll_util(EPOLL_CTL_MOD, c_socket, this, EPOLLOUT);
	}
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

void	Connexion::readHeader() {}
void	Connexion::parseHeader() {}
// bool	Connexion::readBody() {}

t_http_message	&Connexion::			getRequest() { return request; }
