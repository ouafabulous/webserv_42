#include <IO.hpp>
#include <iostream>

IOEvent::IOEvent(
	t_io_result result,
	IO *io_elem,
	const std::string &log,
	const uint http_error)
	: result(result), io_elem(io_elem), log(log), http_error(http_error) {};
IOEvent::IOEvent(const IOEvent &from)
	: result(from.result), io_elem(from.io_elem), log(from.log), http_error(from.http_error) {};
IOEvent::~IOEvent(){};
IOEvent &IOEvent::operator=(const IOEvent &rhs)
{
	// self-assignment guard
	if (this == &rhs)
		return *this;
	result = rhs.result;
	io_elem = rhs.io_elem;
	log = rhs.log;
	http_error = rhs.http_error;
	return *this;
}
