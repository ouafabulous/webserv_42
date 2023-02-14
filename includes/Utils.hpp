#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <Type.hpp>
#include <IO.hpp>
#include <Logger.hpp>

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>

bool			set_nonblocking(int fd);
bool			epoll_util(int action, t_fd fd, IO* io_ptr, int flags);
std::string		http_header_formatter(uint status_code, size_t content_length);
std::string		escape_string(const std::string &str);


std::ostream&	operator<<(std::ostream &out, const t_network_address &netAddr);

#endif
