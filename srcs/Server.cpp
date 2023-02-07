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
			Logger::info << INIT_SUCCESS_HEADER << "listening on " << *it << '\n';
		}
		catch(const std::runtime_error& e) {
			Logger::warning << INIT_ERROR_HEADER << e.what() << " on " << *it << '\n';
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

	if (socks.empty())
		throw std::runtime_error("there is no virtual server listening");
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
				else if (events[i].events & EPOLLIN)
					static_cast<IO*>(events[i].data.ptr)->read();
				else if (events[i].events & EPOLLOUT)
					static_cast<IO*>(events[i].data.ptr)->write();
			}
			catch(const IOEvent& e) {
				if (e.isSuccess())
					Logger::info << e.what() << std::endl;
				else
					Logger::warning << e.what() << std::endl;
				delete socks[e.toDelete()];
				socks.erase(e.toDelete());
			}
		}
	}
}

t_fd							Server::epollfd = -1;
std::map<t_fd, IO*>				Server::socks;
