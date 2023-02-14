#pragma once
#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Parser.hpp"

#include <Type.hpp>
// #include "Socket.hpp"
// #include "Ressource.hpp"

#include <arpa/inet.h>

class Ressource;

class Route
{
	public:
		Route(const t_attributes = t_attributes());
		Route(const Route& ref);
		~Route();
		Route&			operator=(const Route& rhs);
		// void			handle(t_http_message &req) const;
		bool			checkHeader(const t_http_message &req) const;		// let a connexion know if her header is conform to the route
		const Ressource	*createRessource(const t_http_message &req) const;
		std::string		getError(uint http_error) const;

	private:
		t_attributes	attributes;
};

class Router
{
public:
	Router(Parser const &config);
	~Router();

	std::vector<t_network_address>	getAddr() const;																	// get all the address and port to open ListenSockets
	const Route						*getRoute(const t_network_address netAddr, const t_http_message &req) const;		// get route which have to follow a connexion

private:
	typedef std::map<std::string, Route>					vserver_map; 		// <"www.42.fr", *Route>
	typedef std::map<t_network_address, vserver_map>		router_map;			// <192.168.1.10, <"www.42.fr", *Route>>
	router_map							my_map;
};

#endif
