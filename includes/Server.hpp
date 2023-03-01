#ifndef SERVER_HPP
#define SERVER_HPP

#include <Route.hpp>
#include <Router.hpp>
#include <Config.hpp>
#include <Type.hpp>
#include <Errors.hpp>
#include <IO.hpp>
#include <Socket.hpp>
#include <unistd.h>
#include <set>
#include <poll.h>

class Router;
class ListenSocket;

typedef struct pollfd	t_pollfd;

class Server
{
public:
	typedef std::map<t_fd, IO*>							socket_map;
	typedef std::vector<t_pollfd>						t_pollfds_vec;
	typedef t_pollfds_vec::iterator						t_poll_it;

	Server(Parser const &confFile);
	~Server();

	static t_pollfds_vec								pollfds;
	static socket_map									socks;
	void												routine();

private:

	const Router					router;
	typedef std::vector<t_network_address>	listen_list;
};


#endif
