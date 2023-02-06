#pragma once
#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Server.hpp"
#include "Socket.hpp"
#include "Ressource.hpp"

class Route
{
public:
	Route(const t_attributes);
	// void			handle(t_http_message &req) const;
	bool			checkHeader(const t_http_message &req) const;		// let a connexion know if her header is conform to the route
	const Ressource	*createRessource(const t_http_message &req) const;

private:
	t_attributes	attributes;
};

class Router
{
public:
	Router(const std::string &config);
	~Router();

	std::vector<t_network_address>	getAddr() const;								// get all the address and port to open ListenSockets
	const Route						*getRoute(const t_http_message &req) const;		// get route which have to follow a connexion

private:
	typedef std::map<t_network_address,
		std::map<std::string, Route*> >	router_map;									// <192.168.1.10, <"www.42.fr", *Route>>
	router_map							my_map;
};

#endif
