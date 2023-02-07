#include <Router.hpp>

#include <iostream>
#include <set>

Router::Router(const std::string &confile)
{
	(void)confile;
	t_attributes	base;
	my_map[t_network_address(INADDR_ANY, htons(80))]["google.com"] = new Route(base);
	Route	*random_route = new Route(base);
	my_map[t_network_address(INADDR_ANY, htons(8080))]["facebook.com"] = random_route;
	my_map[t_network_address(INADDR_ANY, htons(8088))]["www.facebook.com"] = random_route;

}
// Router::Router(const Router& from)
// :  {
// 	*this = from;
// }
// Router&	Router::operator=(const Router& rhs) {
// 	// self-assignment guard
// 	if (this == &rhs)
// 		return *this;

// 	return *this;
// }
Router::~Router() {
	std::set<Route*>	to_delete;

	for (router_map::const_iterator it = my_map.begin(); it != my_map.end(); it++)
	{
		for (vserver_map::const_iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
			to_delete.insert(it2->second);
	}
	for (std::set<Route*>::const_iterator it = to_delete.begin(); it != to_delete.end(); it++)
		delete *it;
}


std::vector<t_network_address>	Router::getAddr() const {
	std::vector<t_network_address>	response;
	for(router_map::const_iterator it = my_map.begin(); it != my_map.end(); it++)
		response.push_back(it->first);

	return (response);
}

const Route	*Router::getRoute(const t_network_address netAddr, const t_http_message &req) const {
	const vserver_map			&virtual_server = my_map.at(netAddr);
	vserver_map::const_iterator	route = virtual_server.find(req.header_fields.at("Host"));

	if (route == virtual_server.end())
		return (virtual_server.begin()->second);
	return (route->second);
}
