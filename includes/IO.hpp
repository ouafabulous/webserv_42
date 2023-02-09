#pragma once
#ifndef IO_HPP
# define IO_HPP

#include <Type.hpp>
#include <Config.hpp>

class IO;

typedef enum {
	CONTINUE,
	FAIL,
	SUCCESS
}	t_io_result;

struct IOEvent {
	t_io_result	result;
	IO			*io_elem;
	std::string	message;

	IOEvent(t_io_result result = CONTINUE, IO *io_elem = NULL, const std::string &message = "");
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
	char	buffer[BUFFER_SIZE];
};

#endif
