#include <Socket.hpp>

ListenSocket::ListenSocket(const t_network_address netAddr, const Router &router) : l_socket(socket(AF_INET, SOCK_STREAM, 0)),
																					netAddr(netAddr),
																					addr_len(sizeof(address)),
																					router(router)
{
	int reuse = 1;

	if (l_socket < 0)
		throw std::runtime_error("socket creation failed");

	setsockopt(l_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse));

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = netAddr.first;
	address.sin_port = netAddr.second; //	need to htons ?

	if (bind(l_socket, (struct sockaddr *)&address, addr_len) < 0)
		throw std::runtime_error("bind failed");
	if (set_nonblocking(l_socket))
		throw std::runtime_error("set_nonblocking function failed");
	if (listen(l_socket, CONN_TO_LISTEN))
		throw std::runtime_error("listen failed");
	if (epoll_util(EPOLL_CTL_ADD, l_socket, this, EPOLLIN | EPOLLET))
		throw std::runtime_error("epoll_ctl failed");
}

ListenSocket::~ListenSocket() {
	if (epoll_util(EPOLL_CTL_DEL, l_socket, this, EPOLLIN | EPOLLET))
		throw std::runtime_error("epoll_del failed");
	close(l_socket);
}

IOEvent ListenSocket::read()
{
	Connexion *new_conn;

	t_fd client_fd = accept(l_socket, (struct sockaddr *)&address, (socklen_t *)&addr_len);
	if (client_fd == -1)
		return(IOEvent(FAIL, this, "couldn't accept new user"));
	if (set_nonblocking(client_fd))
		return(IOEvent(FAIL, this, "set_nonblocking function failed"));
	new_conn = new Connexion(netAddr, client_fd, router);
	Server::socks.insert(new_conn);
	if (epoll_util(EPOLL_CTL_ADD, client_fd, new_conn, EPOLLIN))
		return IOEvent(FAIL, this, "epoll_ctl failed");
	Logger::info << "new client is now connected" << std::endl;
	return IOEvent();
}
IOEvent ListenSocket::write() { return IOEvent(); }
IOEvent ListenSocket::closed() { return IOEvent(FAIL, this, "listen socket closed"); }
