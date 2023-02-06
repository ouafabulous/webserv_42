#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <Config.hpp>
#include <Errors.hpp>
#include <Type.hpp>
#include <Server.hpp>
#include <Router.hpp>

#include <sys/epoll.h>
#include <unistd.h>

class IO
{
public:
	IO();
	~IO();

	virtual void	read() = 0;		// called when EPOLLIN received
	virtual void	write() = 0;	// called when EPOLLOUT received
	virtual void	closed() = 0;	// called when EPOLLHUP received
	virtual t_fd	fdDelete() = 0;	// get the fd of the Connexion or ListenSocket to delete
};

class Server
{
public:
	Server(const std::string confFile);
	~Server();

	static t_fd					epollfd;
	void						routine();

private:
	typedef std::vector<t_network_address>	listen_list;
	typedef std::map<t_fd, IO*>				socket_map;

	const Router				router;
	static socket_map			socks;
};


#endif
