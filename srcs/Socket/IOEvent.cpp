#include <IO.hpp>
#include <iostream>

IOEvent::IOEvent(
	t_io_result result,
	IO *io_elem,
	const std::string &log,
	const uint error,
	const t_fd client)
	: result(result), io_elem(io_elem), log(log), error(error), client(client) {};
IOEvent::IOEvent(const IOEvent &from)
	: result(from.result), io_elem(from.io_elem), log(from.log), error(from.error), client(from.client) {};
IOEvent::~IOEvent(){};
IOEvent &IOEvent::operator=(const IOEvent &rhs)
{
	// self-assignment guard
	if (this == &rhs)
		return *this;
	result = rhs.result;
	io_elem = rhs.io_elem;
	log = rhs.log;
	error = rhs.error;
	client = rhs.client;
	return *this;
}
