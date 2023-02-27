#ifndef SERVER_HPP
#define SERVER_HPP

#include <Route.hpp>
#include <Router.hpp>
#include <Config.hpp>
#include <Type.hpp>
#include <Errors.hpp>
#include <IO.hpp>
#include <Socket.hpp>
#include <sys/epoll.h>
#include <unistd.h>
#include <set>

class Router;
class ListenSocket;

class Server
{
public:
	typedef std::set<IO*>	socket_set;

	Server(Parser const &confFile);
	~Server();

	static t_fd						epollfd;
	static socket_set				socks;
	void							routine();

private:
	const Router					router;
	typedef std::vector<t_network_address>	listen_list;
};


#endif
