#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Parser.hpp"
#include "Type.hpp"
#include <Utils.hpp>
#include <arpa/inet.h>
// #include <Ressource.hpp>
#include <Socket.hpp>
#include <Route.hpp>

// class Ressource;
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
