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
	set_nonblocking(l_socket);
	if (listen(l_socket, CONN_TO_LISTEN))
		throw std::runtime_error("listen failed");
	epoll_util(EPOLL_CTL_ADD, l_socket, this, EPOLLIN | EPOLLET);
}

ListenSocket::~ListenSocket() {
	epoll_util(EPOLL_CTL_DEL, l_socket, this, EPOLLIN);
	close(l_socket);
}

void ListenSocket::read()
{
	Connexion *new_conn;

	t_fd client_fd = accept(l_socket, (struct sockaddr *)&address, (socklen_t *)&addr_len);
	if (client_fd == -1)
		throw std::runtime_error("couldn't accept new user");
	set_nonblocking(client_fd);
	new_conn = new Connexion(netAddr, client_fd, router);
	Server::socks[client_fd] = new_conn;
	epoll_util(EPOLL_CTL_ADD, client_fd, new_conn, EPOLLIN | EPOLLOUT);
	std::cout << "new client is now connected" << std::endl;
}
void ListenSocket::write() {}
void ListenSocket::closed() { throw std::runtime_error("Listen socket closed"); }
t_fd ListenSocket::fdDelete() { return (l_socket); }
