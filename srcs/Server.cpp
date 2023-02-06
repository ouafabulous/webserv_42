#include "Server.hpp"
#include <iostream>

Server::Server(const std::string config_file)
: router(config_file)
{
	// epollfd = epoll_create(EPOLL_BACKLOG);
	// if (epollfd == -1)
	// 	throw std::exception();

	// std::vector<t_network_address>	listen_socket_to_build = router.getAddr();
	// for (std::vector<t_network_address>:: = 0; i < count; i++)
	// {
	// 	/* code */
	// }

}
Server::~Server() {
}

void	Server::routine() {

}

