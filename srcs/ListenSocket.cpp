#include "Socket.hpp"

ListenSocket::ListenSocket(const t_network_address netAddr) : l_socket(socket(AF_INET, SOCK_STREAM, 0))
{
	struct sockaddr_in address;
	epoll_event tmp_epoll_event;
	int addr_len = sizeof(address);
	int reuse = 1;

	try
	{
		if (l_socket < 0)
			throw std::runtime_error("Listen socket failed to create.");

		setsockopt(l_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse));

		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = netAddr.second; //	need to htons ?

		if (bind(l_socket, (struct sockaddr *)&address, addr_len) < 0)
			throw std::runtime_error("Bind failed.");
		if (set_nonblocking(l_socket) == -1)
			throw std::runtime_error("set_nonblocking function failed.");
		if (listen(l_socket, CONN_TO_LISTEN))
			throw std::runtime_error("Listen failed.");

		tmp_epoll_event.data.fd = l_socket;
		tmp_epoll_event.events = EPOLLIN | EPOLLET;

		if (epoll_ctl(Server::epollfd, EPOLL_CTL_ADD, l_socket, &tmp_epoll_event) == -1)
			throw std::runtime_error("epoll_ctl failed.");
	}
	catch (const std::runtime_error& e)
	{
		perror(e.what());
		exit(EXIT_FAILURE);
	}
}

ListenSocket::~ListenSocket() {}
void	ListenSocket::read() {}
void	ListenSocket::write() {}
void	ListenSocket::closed() {}
t_fd	ListenSocket::fdDelete() {}
