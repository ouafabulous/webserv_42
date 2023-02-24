#pragma once
#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Parser.hpp"
#include "Type.hpp"
#include "Connexion.hpp"
#include <Utils.hpp>
#include <arpa/inet.h>
#include <Ressource.hpp>

class Ressource;


	class Route
{
	public:
		Route(const t_attributes = t_attributes());
		Route(const Route& ref);
		~Route();
		Route&			operator=(const Route& rhs);
		void			handle(t_http_message &req,Connexion *connexion) const;
		bool			checkRequest(const t_http_message &req) const;		// let a connexion know if her header is conform to the route
		IOEvent			setRessource(const t_http_message &req, Connexion *connexion) const;
		std::string		getError(uint http_error) const;
		size_t			getMaxBodySize() const;
		void			printAttributes() const;
		t_attributes const &getAttributes() const;


	private:
		t_attributes	attributes;
};

typedef struct s_vserver {
  std::vector<std::string>	_server_names;
  std::vector<Route>		_routes;
  s_vserver(std::vector<std::string> server_names)
    : _server_names(server_names){ }
} t_vserver;


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
	typedef std::vector<t_vserver>	t_vserver_vec;
	typedef std::map<t_network_address, t_vserver_vec>	t_network_map;

	t_network_map					_network_map;
	const t_vserver&				findVserver(const t_network_address addr, const std::string& host) const;
	bool							isInServerName(const std::string& host, const std::vector<std::string>	server_names) const;
};


#endif
