#pragma once
#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Server.hpp"
#include "Socket.hpp"
#include "Ressource.hpp"

class Route
{
public:
	Route();
	// void								handle(t_http_message &req) const;
	const Ressource						*create_ressource(const t_http_message &req) const;

private:
	typedef enum : char {
		GET = 0b001,
		POST = 0b010,
		DELETE = 0b100
	}	t_methods;

	t_methods							methods_allowed;	// could be GET POST DELETE
	std::vector<std::string>			server_names;		// defined by header field "Host"
	size_t								max_body_length;	
	std::map<uint, std::string>			error_files;		// path to default error pages
	std::string							redirect;			// uri for redirection, 
	std::string							root;				// path to root directory for serving static files
	std::map<std::string, std::string>	cgi_path;			// path to CGI locations
	bool								directory_listing;	// autoindex on/off
};

class Router
{
public:
	Router(std::string &config);
	~Router();

	std::vector<t_network_address>		getAddr() const;								// get all the address and port to open ListenSockets
	const Route							*getRoute(const t_http_message &req) const;		// get route which have to follow a connexion
	bool								checkHeader(const t_http_message &req) const;	// let a connexion know if her header is conform to the route

private:
	typedef std::map<t_network_address,
		std::map<std::string, Route>>	router_map;
	router_map							my_map;
};

#endif
