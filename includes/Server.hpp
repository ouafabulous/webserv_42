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

class ListenSocket;

class Server
{
public:
	typedef std::map<t_fd, IO*>				socket_map;

	Server(const std::string confFile);
	~Server();

	static t_fd					epollfd;
	static socket_map			socks;
	void						routine();

private:
	typedef std::vector<t_network_address>	listen_list;

	const Router				router;
};


#endif
