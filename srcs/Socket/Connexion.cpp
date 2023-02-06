#include "../includes/Socket.hpp"

Connexion::Connexion(const t_network_address netAddr,
	const t_fd socket, const Router &router) :	c_socket(socket),
												netAddr(netAddr),
												router(router)
{}

Connexion::~Connexion() {}
void	Connexion::read() {}
void	Connexion::write() {}
void	Connexion::closed() {}
t_fd	Connexion::fdDelete() {}

void	Connexion::readHeader() {}
void	Connexion::parseHeader() {}
bool	Connexion::readBody() {}
