#pragma once
#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <Type.hpp>

#include <exception>
#include <iostream>

typedef enum {
	FAIL,
	SUCCESS
}	t_io_state;

class IOEvent : public std::runtime_error {
private:
	const t_io_state	success;
	const t_fd			to_delete;
public:
	IOEvent(const t_io_state success, const t_fd to_delete, const std::string &message)
		: std::runtime_error(message),
		success(success),
		to_delete(to_delete) { }

	t_fd		toDelete() const {return (to_delete);}
	t_io_state	isSuccess() const {return (success);}
};

#endif
