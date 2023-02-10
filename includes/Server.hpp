#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <Config.hpp>
#include <Errors.hpp>
#include <Type.hpp>
#include <Router.hpp>
#include <IO.hpp>
#include <Socket.hpp>

#include <sys/epoll.h>
#include <unistd.h>
#include <set>

class ListenSocket;

class Server
{
public:
	typedef std::set<IO*>	socket_set;

	Server(const std::string confFile);
	~Server();

	static t_fd					epollfd;
	static socket_set			socks;
	void						routine();

private:
	typedef std::vector<t_network_address>	listen_list;

	const Router				router;
};


#endif
