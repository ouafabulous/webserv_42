#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include "Type.hpp"
#include "Server.hpp"
#include "Router.hpp"

class Server
{
public:
	Server(std::string confFile);
	~Server();

	const static int			epollfd;
	void						routine();

private:
	const Router				router;
	static std::map<t_fd, IO *>	socks;
};

class IO
{
public:
	IO();
	~IO();

	virtual void	read() = 0;			// called when EPOLLIN received
	virtual void	write() = 0;		// called when EPOLLOUT received
	virtual void	closed() = 0;		// called when EPOLLHUP received
	virtual t_fd	fd_delete() = 0;	// get the fd of the Connexion or ListenSocket to delete
};

#endif
