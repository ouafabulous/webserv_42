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
			socks.insert(current_listen_socket);
			Logger::info << "listening on " << *it << '\n';
		}
		catch(const std::runtime_error& e) {
			Logger::warning << e.what() << " on " << *it << '\n';
		}
	}

	// ERRORS DEFINITIONS
	errors[404] = "HTTP/1.1 404 OK\nContent-Type:text/html\nContent-Length: 49\n\n<html><body><h1>File not found</h1></body></html>";
	errors[413] = "413 Payload Too Large";
}

Server::~Server() {
	for (socket_set::const_iterator it = socks.begin(); it != socks.end(); it++)
		delete	*it;
	if (close(epollfd))
		throw std::runtime_error("Unable to close epollfd");
}

void	Server::routine() {
	int				epoll_wait_return;
	epoll_event		events[EPOLL_BACKLOG];
	IOEvent			io_event;

	if (socks.empty())
		throw std::runtime_error("there is no virtual server listening");
	while (TRUE)
	{
		epoll_wait_return = epoll_wait(epollfd, events, EPOLL_BACKLOG, -1);
		if (epoll_wait_return == -1)
			throw std::runtime_error("Epoll wait return -1");
		for (int i = 0; i < epoll_wait_return; i++)
		{
			if (events[i].events & EPOLLHUP || events[i].events & EPOLLRDHUP)
				io_event = static_cast<IO*>(events[i].data.ptr)->closed();
			else if (events[i].events & EPOLLIN)
				io_event = static_cast<IO*>(events[i].data.ptr)->read();
			else if (events[i].events & EPOLLOUT)
				io_event = static_cast<IO*>(events[i].data.ptr)->write();
			// check result of IO
			if (io_event.result == ERROR) {
				std::string	&error_message = errors[io_event.error];
				send(io_event.client, error_message.c_str(), error_message.length(), MSG_DONTWAIT);
			}
			if (io_event.result) {
				if (io_event.result == SUCCESS)
					Logger::info << io_event.log << std::endl;
				else
					Logger::warning << io_event.log << std::endl;
				delete io_event.io_elem;
				socks.erase(io_event.io_elem);
			}
		}
	}
}

t_fd			Server::epollfd = -1;
std::set<IO*>	Server::socks;
char			IO::buffer[BUFFER_SIZE];
