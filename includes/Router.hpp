#pragma once
#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Parser.hpp"
#include "Type.hpp"
#include <Utils.hpp>
#include <arpa/inet.h>
#include <Ressource.hpp>
#include <Socket.hpp>
#include <Route.hpp>

class Ressource;
class Connexion;
class Route;

struct NetworkRoute
{
	t_network_address _address;
	std::vector<std::string> _server_name;
	Route _route;
	NetworkRoute(t_network_address address_, std::vector<std::string> string_list_, Route route_)
		: _address(address_), _server_name(string_list_), _route(route_)
	{
	}
};


class Router
{
public:
	Router(Parser const &config);
	~Router();

	std::vector<t_network_address> getAddr() const;											 // get all the address and port to open ListenSockets
	const Route *getRoute(const t_network_address netAddr, const t_http_message &req) const; // get route which have to follow a connexion

	void fillAttributes(t_attributes *attributes, std::vector<Directive> const &directives);
	void printRoutes() const;

private:
	std::vector<NetworkRoute> _networkRoutes;
	bool isInServerName(const std::string &host, const std::vector<std::string> server_names) const;
};


#endif
