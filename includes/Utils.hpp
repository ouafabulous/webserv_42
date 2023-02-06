#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>

int	set_nonblocking(int fd);

#endif
