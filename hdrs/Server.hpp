#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

#include "Type.hpp"
#include "Server.hpp"
#include "Router.hpp"

class Server
{
	public:
		Server(std::string confFile);
		static std::map<t_fd, IO*> socks;
		static int epollfd;
		Router router;
		void routine();
};


class IO
{
	public:
		IO();
		virtual void read() = 0;
		virtual void write() = 0;
		virtual void closed() = 0;
		virtual int fd_delete() = 0;
};

#endif
