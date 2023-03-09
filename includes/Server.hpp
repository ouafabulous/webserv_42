#ifndef SERVER_HPP
#define SERVER_HPP

#include <Route.hpp>
#include <Router.hpp>
#include <Config.hpp>
#include <Type.hpp>
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
	/**
	 * @brief map that contains all the sockets, with their fd as key and IO* as value
	 *
	 */
	typedef std::map<t_fd, IO*>							socket_map;

	/**
	 * @brief vector that contains all the fd used by poll()
	 *
	 */
	typedef std::vector<t_pollfd>						t_pollfds_vec;
	typedef t_pollfds_vec::iterator						t_poll_it;

	/**
	 * @brief Construct a new Server object
	 *
	 * @param confFile
	 */
	Server(Parser const &confFile);
	/**
	 * @brief Destroy the Server object
	 *
	 */
	~Server();

	static t_pollfds_vec								pollfds;
	static socket_map									socks;
	/**
	 * @brief busy loop that will run the server, each loop take poll() returned value and call the right IO functions
	 *
	 */
	void												routine();

private:

	const Router					router;
	typedef std::vector<t_network_address>	listen_list;
};


#endif
