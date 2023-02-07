#include <IO.hpp>
#include <iostream>

IOEvent::IOEvent(t_io_result result, IO *io_elem, const std::string &message)
	: result(result), io_elem(io_elem), message(message){};
IOEvent::IOEvent(const IOEvent &from)
	: result(from.result), io_elem(from.io_elem), message(from.message){};
IOEvent::~IOEvent(){};
IOEvent&	IOEvent::operator=(const IOEvent& rhs) {
	// self-assignment guard
	if (this == &rhs)
		return *this;
	result = rhs.result;
	io_elem = rhs.io_elem;
	message = rhs.message;
	return *this;
}
