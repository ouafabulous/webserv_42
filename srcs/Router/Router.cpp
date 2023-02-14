#include <Router.hpp>

#include <iostream>
#include <set>

Router::Router(Parser const &confile)
{
	(void)confile;
	t_attributes	base;
	my_map[t_network_address(INADDR_ANY, htons(8080))]["facebook.com"] = Route();
	base.error_files.addError(404, "./README.md");
	Route	random_route(base);
	my_map[t_network_address(INADDR_ANY, htons(8088))]["www.facebook.com"] = random_route;
}
Router::~Router() {
}


std::vector<t_network_address>	Router::getAddr() const {
	std::vector<t_network_address>	response;
	for(router_map::const_iterator it = my_map.begin(); it != my_map.end(); it++)
		response.push_back(it->first);

	return (response);
}

const Route	*Router::getRoute(const t_network_address netAddr, const t_http_message &req) const {
	const vserver_map			&virtual_server = my_map.at(netAddr);
	vserver_map::const_iterator	route;

	if (req.header_fields.find("Host") == req.header_fields.end())
		return &virtual_server.begin()->second;
	return &virtual_server.find(req.header_fields.at("Host"))->second;
}
