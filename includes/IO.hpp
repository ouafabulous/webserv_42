#ifndef IO_HPP
# define IO_HPP

#include <Type.hpp>
#include <Config.hpp>
#include <poll.h>

class IO;

typedef enum {
	CONTINUE,		// CONTINUE TO THE NEXT IO EVENT
	FAIL,			// DELETE CONN
	SUCCESS			// DELETE CONN
}	t_io_result;

struct IOEvent {
	t_io_result	result;		// IO MANIPULATION RESULT
	IO			*io_elem;	// PTR TO THE IO TO DESTROY IN CASE OF FAILURE
	std::string	log;		// LOG TO PRINT SERVER SIDE
	uint		http_error;		// ERROR TO SEND CLIENT SIDE

	IOEvent(t_io_result result = CONTINUE, IO *io_elem = NULL, const std::string &log = "", const uint http_error = 0);
	IOEvent(const IOEvent& ref);
	~IOEvent();
	IOEvent&	operator=(const IOEvent& rhs);
};

class IO
{
public:
	virtual ~IO() {};
	virtual IOEvent	read() {return IOEvent(FAIL, this, "Unknown error");};		// called when POLLIN received
	virtual IOEvent	write() {return IOEvent(FAIL, this, "Unknown error");};	// called when POLLOUT received
	virtual IOEvent	closed() {return IOEvent(FAIL, this, "Unknown error");};	// called when POLLHUP received

protected:
	char	buffer[BUFFER_SIZE + 1];
};

#endif
