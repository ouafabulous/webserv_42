#pragma once
#ifndef TYPE_HPP
# define TYPE_HPP

#include <string>
#include <map>
#include <vector>
#include <netinet/in.h>

typedef int t_fd;

struct t_request_line {
	std::string							method;
	std::string							path;
	std::string							http_version;
};

struct t_http_message {
	t_request_line						request_line;
	std::map<std::string, std::string>	header_fields;
	std::vector<char>					body;
};

struct t_network_address {
	in_addr_t							address;
	in_port_t							port;
};

#endif