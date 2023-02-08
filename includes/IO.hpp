#pragma once
#ifndef IO_HPP
# define IO_HPP

#include <Type.hpp>
#include <Config.hpp>

class IO;

typedef enum {
	CONTINUE,		// CONTINUE TO THE NEXT IO EVENT
	ERROR,			// WRITE ERROR TO THE CLIENT AND THEN DELETE CONN
	FAIL,			// DELETE CONN
	SUCCESS			// DELETE CONN
}	t_io_result;

struct IOEvent {
	t_io_result	result;		// IO MANIPULATION RESULT
	IO			*io_elem;	// PTR TO THE IO TO DESTROY IN CASE OF FAILURE
	std::string	log;		// LOG TO PRINT SERVER SIDE
	uint		error;		// ERROR TO SEND CLIENT SIDE
	t_fd		client;			// FD TO SEND ERROR TO THE CLIENT

	IOEvent(t_io_result result = CONTINUE, IO *io_elem = NULL, const std::string &log = "", const uint error = 0, const t_fd client = -1);
	IOEvent(const IOEvent& ref);
	~IOEvent();
	IOEvent&	operator=(const IOEvent& rhs);
};

class IO
{
public:
	virtual ~IO() {};
	virtual IOEvent	read() = 0;		// called when EPOLLIN received
	virtual IOEvent	write() = 0;	// called when EPOLLOUT received
	virtual IOEvent	closed() = 0;	// called when EPOLLHUP received

protected:
	static char	buffer[BUFFER_SIZE];
};

#endif
