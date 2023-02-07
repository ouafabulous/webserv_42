#include <Server.hpp>

#define TRUE 1

Server::Server(const std::string config_file)
: router(config_file)
{
	epollfd = epoll_create(EPOLL_BACKLOG);
	if (epollfd == -1)
		throw std::runtime_error("Unable to create epollfd");

	listen_list		lsocket_to_build = router.getAddr();
	ListenSocket	*current_listen_socket;
	for (listen_list::const_iterator it = lsocket_to_build.begin(); it != lsocket_to_build.end(); it++)
	{
		try {
			current_listen_socket = new ListenSocket(*it, router);
			socks[current_listen_socket->fdDelete()] = current_listen_socket;
			std::cout << INIT_SUCCESS_HEADER << "listening on " << *it << '\n';
		}
		catch(const std::runtime_error& e) {
			std::cerr << INIT_ERROR_HEADER << e.what() << " on " << *it << '\n';
		}
	}
}

Server::~Server() {
	for (socket_map::const_iterator it = socks.begin(); it != socks.end(); it++)
		delete	it->second;
	if (close(epollfd))
		throw std::runtime_error("Unable to close epollfd");
}

void	Server::routine() {
	int				epoll_wait_return;
	epoll_event		events[EPOLL_BACKLOG];
	t_fd			to_delete;

	if (socks.empty())
		return;
	while (TRUE)
	{
		epoll_wait_return = epoll_wait(epollfd, events, EPOLL_BACKLOG, -1);
		if (epoll_wait_return == -1)
			throw std::runtime_error("Epoll wait return -1");
		for (int i = 0; i < epoll_wait_return; i++)
		{
			try {
				if (events[i].events & EPOLLHUP || events[i].events & EPOLLRDHUP)
					static_cast<IO*>(events[i].data.ptr)->closed();
				if (events[i].events & EPOLLIN)
					static_cast<IO*>(events[i].data.ptr)->read();
				if (events[i].events & EPOLLOUT)
					static_cast<IO*>(events[i].data.ptr)->write();
			}
			catch(const std::exception& e) {
				std::cerr << ROUTINE_ERROR_HEADER << e.what() << '\n';
				to_delete = static_cast<IO*>(events[i].data.ptr)->fdDelete();
				delete socks[to_delete];
				socks.erase(to_delete);
			}
		}
	}
}

t_fd							Server::epollfd = -1;
std::map<t_fd, IO*>				Server::socks;
