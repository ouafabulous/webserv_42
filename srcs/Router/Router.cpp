#include <Router.hpp>

#include <iostream>
#include <set>


int i = 0;

Router::Router(Parser const &confile)
{
	t_attributes attributes = {};
	BlockServer *tmp1 = confile.getBlock();
	while (tmp1)
	{
		fillAttributes(&attributes, tmp1->getDirectives());
		std::vector<BlockLocation *>::const_iterator it;
		std::vector<BlockLocation *> const &locations = tmp1->getChilds();
		for (it = locations.begin(); it != locations.end(); it++)
		{
			attributes.location = (*it)->getLocationValue();
			fillAttributes(&attributes, (*it)->getDirectives());
			Route	route(attributes);
			// std::cout <<"address: " << t_network_address(INADDR_ANY, htons(attributes.port)) << std::endl;
			// std::cout << "server name: " << attributes.server_name << std::cout;
			my_map[t_network_address(INADDR_ANY, htons(attributes.port))][attributes.server_name] = route; // IADDR_ANY consideres that we listen on all ports aka 0.0.0.0
		}

			// std::cout << "got here with this port: " << attributes.port << std::endl;
		tmp1 = tmp1->getSibling();

	}
}

Router::~Router()
{
}

// std::ostream &operator<<(std::ostream &os, const t_network_address &addr)
// {
// 	// Convert the IP address to a string
// 	char ip_str[INET_ADDRSTRLEN];
// 	inet_ntop(AF_INET, &(addr.first), ip_str, INET_ADDRSTRLEN);

// 	// Print the IP address and port
// 	os << ip_str << ":" << ntohs(addr.second);

// 	return os;
// }

void Router::printRoutes() const
{
	std::cout << "size_of_router" << my_map.size() << std::endl;	
	router_map::const_iterator router_iter;
	for (router_iter = my_map.begin(); router_iter != my_map.end(); ++router_iter)
	{
		std::cout << "--------------" << std::endl;
		std::cout << "Router address: " << router_iter->first << std::endl;
		// iterate through the elements of the vserver_map for each router address
		const vserver_map &vserver_map_ref = router_iter->second;
		for (vserver_map::const_iterator vserver_iter = vserver_map_ref.begin(); vserver_iter != vserver_map_ref.end(); ++vserver_iter)
		{
			std::cout << "  Virtual server name: " << vserver_iter->first << std::endl;
			// print the details of the Route object
			const Route &route = vserver_iter->second;
			std::cout << "    Route properties: " << std::endl;
			route.printAttributes();
			// print other properties of the Route object here
		}
		std::cout << "--------------" << std::endl;
	}
}

t_methods operator|=(t_methods& a, t_methods b)
{
    return a = static_cast<t_methods>(a | b);
}

void Router::fillAttributes(t_attributes *attributes, std::vector<directive> const &directives)
{
	std::map<std::string, t_methods> methodsDict;

    // Add some entries to the dictionary
    methodsDict["GET"] = GET;
    methodsDict["POST"] = POST;
    methodsDict["DELETE"] = DELETE;
	std::vector<directive>::const_iterator it;
	for (it = directives.begin(); it != directives.end(); it++)
	{
		if (it->first == "allowed_methods")
		{
			attributes->allowed_methods |= methodsDict[it->second.getDirectiveValue()._stringValue]; //we are supposing here that we can get only one allowed_method at once, while in reality we can have up to 3 and then I gotta iterate all over them
		}
			else if (it->first == "listen")
		{
			attributes->port = static_cast<uint>(it->second.getDirectiveValue()._intValue);
		}
		else if (it->first == "server_name")
		{
			attributes->server_name = it->second.getDirectiveValue()._stringValue;
			// attributes->server_names.push_back(it->second.getDirectiveValue()._stringValue);
		}
		else if (it->first == "client_max_body_size")
		{
			attributes->max_body_length = static_cast<size_t>(it->second.getDirectiveValue()._intValue);
		}
		else if (it->first == "redirect")
		{
			attributes->redirect = it->second.getDirectiveValue()._stringValue;
		}
		else if (it->first == "root")
		{
			attributes->root = it->second.getDirectiveValue()._stringValue;
		}
		else if (it->first == "auto-index")
		{
			(it->second.getDirectiveValue()._stringValue == std::string("on")) ? attributes->directory_listing = true : attributes->directory_listing = false;
		}
	}
}

std::vector<t_network_address> Router::getAddr() const
{
	std::vector<t_network_address> response;
	for (router_map::const_iterator it = my_map.begin(); it != my_map.end(); it++)
		response.push_back(it->first);

	return (response);
}

const Route *Router::getRoute(const t_network_address netAddr, const t_http_message &req) const
{
	const vserver_map &virtual_server = my_map.at(netAddr);
	vserver_map::const_iterator route;

	if (req.header_fields.find("Host") == req.header_fields.end())
		return &virtual_server.begin()->second;
	return &virtual_server.find(req.header_fields.at("Host"))->second;
}
