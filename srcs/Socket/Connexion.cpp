#include <Socket.hpp>

Connexion::Connexion(const t_network_address netAddr,
	const t_fd socket, const Router &router) :	c_socket(socket),
												netAddr(netAddr),
												router(router)
{}

Connexion::~Connexion() {
	epoll_util(EPOLL_CTL_DEL, c_socket, this, EPOLLIN);
	close(c_socket);
}
void	Connexion::read() {}
void	Connexion::write() {
	std::string text = "HTTP/1.1 200 OK\nContent-Type:text/plain\nContent-Length: 12\n\nHello World!";
	send(c_socket, text.c_str(), text.length(), MSG_DONTWAIT);
}
void	Connexion::closed() { throw std::runtime_error("Connexion ended"); }
t_fd	Connexion::fdDelete() { return (c_socket); }

void	Connexion::readHeader() {}
void	Connexion::parseHeader() {}
// bool	Connexion::readBody() {}
