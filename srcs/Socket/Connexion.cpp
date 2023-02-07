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
void	Connexion::read() {
	char	buffer[BUFFER_SIZE];
	ssize_t	recv_size = -1;
	if ((recv_size = recv(c_socket, buffer, BUFFER_SIZE, MSG_DONTWAIT)) > 0) {
		buffer[recv_size -1] = 0;
		request_header.append(buffer);
	}
	if (ends_with(request_header, "\n\r")) {
		// std::cout << request_header << std::endl;
		Logger::info << "request ok" << std::endl;
		epoll_util(EPOLL_CTL_MOD, c_socket, this, EPOLLOUT);
	}
}
void	Connexion::write() {
	std::string text = "HTTP/1.1 200 OK\nContent-Type:text/plain\nContent-Length: 12\n\nHello World!";
	send(c_socket, text.c_str(), text.length(), MSG_DONTWAIT);
}
void	Connexion::closed() { throw std::runtime_error("Connexion ended"); }
t_fd	Connexion::fdDelete() { return (c_socket); }

void	Connexion::readHeader() {}
void	Connexion::parseHeader() {}
// bool	Connexion::readBody() {}
