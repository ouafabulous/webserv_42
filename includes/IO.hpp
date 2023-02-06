#pragma once
#ifndef IO_HPP
# define IO_HPP

#include <Type.hpp>

class IO
{
public:
	virtual ~IO() {};
	virtual void	read() = 0;		// called when EPOLLIN received
	virtual void	write() = 0;	// called when EPOLLOUT received
	virtual void	closed() = 0;	// called when EPOLLHUP received
	virtual t_fd	fdDelete() = 0;	// get the fd of the Connexion or ListenSocket to delete
};

#endif
