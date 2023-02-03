#pragma once
#ifndef TYPE_HPP
#define TYPE_HPP

#include <string>
#include <map>
#include <vector>
#include <netinet/in.h>

// IO

typedef int t_fd;

struct t_request_line
{
	std::string method;
	std::string path;
	std::string http_version;
};

struct t_http_message
{
	t_request_line request_line;
	std::map<std::string, std::string> header_fields;
	std::vector<char> body;
};

struct t_network_address
{
	in_addr_t address;
	in_port_t port;
};

// ROUTER && ROUTE

typedef enum : char {
	GET = 0b001,
	POST = 0b010,
	DELETE = 0b100
}	t_methods;

typedef struct s_attributes {
	t_methods							methods_allowed;	// could be GET POST DELETE
	std::vector<std::string>			server_names;		// defined by header field "Host"
	size_t								max_body_length;
	std::map<uint, std::string>			error_files;		// path to default error pages
	std::string							redirect;			// uri for redirection,
	std::string							root;				// path to root directory for serving static files
	std::map<std::string, std::string>	cgi_path;			// path to CGI locations
	bool								directory_listing;	// autoindex on/off
}	t_attributes;

#endif
