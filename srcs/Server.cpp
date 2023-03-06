#include <Router.hpp>
#include <Server.hpp>


#define TRUE 1

Server::Server(Parser const &config_file)
: router(config_file)
{
	listen_list		lsocket_to_build = router.getAddr();
	for (listen_list::const_iterator it = lsocket_to_build.begin(); it != lsocket_to_build.end(); it++)
	{
		try {
			new ListenSocket(*it, router);
			Logger::info << "listening on " << *it << '\n';
		}
		catch(const std::runtime_error& e) {
			Logger::warning << e.what() << " on " << *it << '\n';
		}
	}
}

Server::~Server() {
	for(Server::socket_map::iterator it = socks.begin(); it != socks.end(); it++)
	{
		if (it->second)
			delete it->second;
	}
}

void	Server::routine() {
	socket_map::iterator		io;
	int							poll_wait_return;
	IOEvent						io_event;
	t_pollfds_vec				curr_fds;


	if (socks.empty())
		throw std::runtime_error("there is no virtual server listening\n");
	while (TRUE)
	{
		curr_fds = pollfds;
		poll_wait_return = poll(&curr_fds[0], curr_fds.size(), -1);
		if (poll_wait_return == -1)
			throw std::runtime_error("Poll wait return -1\n");
		for (t_poll_it it = curr_fds.begin(); it != curr_fds.end(); it++)
		{
			io_event.result = CONTINUE;
			if (it->revents) {
				io = socks.find(it->fd);
				if (io == socks.end())
					continue;
				if (it->revents & POLLIN)
					io_event = io->second->read();
				else if (it->revents & POLLOUT)
					io_event = io->second->write();
				else if (it->revents & POLLHUP || it->revents & POLLRDHUP)
					io_event = io->second->closed();
				// check result of IO
				if (io_event.result) {
					if (io_event.result == SUCCESS)
						Logger::debug << io_event.log << std::endl;
					else
						Logger::warning << io_event.log << std::endl;
					delete io_event.io_elem;
				}
			}
		}
	}
}

Server::t_pollfds_vec		Server::pollfds;
Server::socket_map			Server::socks;
