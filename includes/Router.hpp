#pragma once
#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Parser.hpp"
#include "Type.hpp"
#include <Utils.hpp>
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
		size_t			getMaxBodySize() const;
		void			printAttributes() const;


	private:
		t_attributes	attributes;
};

struct NetworkRoute {
  t_network_address			address;
  std::vector<std::string>	server_name;
//   t_attributes				att={};
  Route						route;
  NetworkRoute(t_network_address address_, std::vector<std::string> string_list_)
    : address(address_), server_name(string_list_) {
  }
};


class Router
{
public:
	Router(Parser const &config);
	~Router();

	std::vector<t_network_address>	getAddr() const;																	// get all the address and port to open ListenSockets
	const Route						*getRoute(const t_network_address netAddr, const t_http_message &req) const;		// get route which have to follow a connexion

	void							fillAttributes(t_attributes *attributes, std::vector<Directive>  const &directives);
	void							printRoutes() const;

private:
	std::vector<NetworkRoute>		_networkRoutes;
};


#endif
