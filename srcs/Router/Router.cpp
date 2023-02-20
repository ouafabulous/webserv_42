
#include <iostream>
#include <set>
#include "Type.hpp"
#include "Router.hpp"


// int i = 0;
Router::Router(Parser const &confile)
{
	t_attributes 	attributes = {};
	BlockServer 	*tmp1 = confile.getBlock();
	while (tmp1)
	{
		fillAttributes(&attributes, tmp1->getDirectives());
		std::vector<BlockLocation *>::const_iterator it;
		std::vector<BlockLocation *> const &locations = tmp1->getChilds();
		Route	globalRoute(attributes);
		NetworkRoute	globalNetworkRoute(t_network_address(INADDR_ANY, htons(attributes.port)), attributes.server_name, globalRoute);
		_networkRoutes.push_back(globalNetworkRoute);
		for (it = locations.begin(); it != locations.end(); it++)
		{
			attributes.location = (*it)->getLocationValue();
			fillAttributes(&attributes, (*it)->getDirectives());
			Route			route(attributes);
			NetworkRoute	networkRoute(t_network_address(INADDR_ANY, htons(attributes.port)), attributes.server_name, globalRoute);
			_networkRoutes.push_back(networkRoute);
		}

		tmp1 = tmp1->getSibling();
	}
	printRoutes();
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
	// std::cout << "size_of_router: " << my_map.size() << std::endl;
	std::vector<NetworkRoute>::const_iterator	iterRouter;
	for (iterRouter = _networkRoutes.begin(); iterRouter != _networkRoutes.end(); ++iterRouter)
	{
			std::cout << "\n\n\n\n\n------------------------------\n\n\n\n\n" <<  std::endl;
			std::cout << "---Adresse: " << iterRouter->address << std::endl;
			std::cout << "---Server Names: ";
			for (std::vector<std::string>::iterator servNameIter = iterRouter->begin(); servNameIter != iterRouter->end(); servNameIter++){
				std::cout << servNameIter << " ";
			}
			std::cout << "---Route: ";
			iterRouter->route.printAttributes();
		std::cout << "--------------" << std::endl;
	}
}

t_methods operator|=(t_methods& a, t_methods b)
{
    return a = static_cast<t_methods>(a | b);
}

void Router::fillAttributes(t_attributes *attributes, std::vector<Directive> const &directives)
{
	std::map<std::string, t_methods> methodsDict;

    // Add some entries to the dictionary
    methodsDict["GET"] = GET;
    methodsDict["POST"] = POST;
    methodsDict["DELETE"] = DELETE;
	std::vector<Directive>::const_iterator it;
	std::vector<directiveValueUnion>::const_iterator	directiveValIter;
	for (it = directives.begin(); it != directives.end(); it++)
	{
		if (it->getDirectiveName() == "allowed_methods")
		{
			for (directiveValIter = (it->getDirectiveValues()).begin(); directiveValIter != (it->getDirectiveValues()).end(); directiveValIter++){
				attributes->allowed_methods |= methodsDict[directiveValIter->_stringValue]; //we are supposing here that we can get only one allowed_method at once, while in reality we can have up to 3 and then I gotta iterate all over them
			}			
		}
			else if (it->getDirectiveName() == "listen")
		{
			attributes->port = static_cast<uint>((it->getDirectiveValues())[0]._intValue);
		}
		else if (it->getDirectiveName() == "server_name")
		{
			for (directiveValIter = (it->getDirectiveValues()).begin(); directiveValIter != (it->getDirectiveValues()).end(); directiveValIter++){
				attributes->server_name.push_back(directiveValIter->_stringValue);
			}
		}
		else if (it->getDirectiveName() == "client_max_body_size")
		{
			attributes->max_body_length = static_cast<size_t>((it->getDirectiveValues())[0]._intValue);
		}
		else if (it->getDirectiveName() == "redirect")
		{
			attributes->redirect = (it->getDirectiveValues())[0]._stringValue;
		}
		else if (it->getDirectiveName() == "root")
		{
			attributes->root = (it->getDirectiveValues())[0]._stringValue
		}
		else if (it->getDirectiveName() == "auto-index")
		{
			((it->getDirectiveValues())[0]._stringValue == std::string("on")) ? attributes->directory_listing = true : attributes->directory_listing = false;
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
	if (virtual_server.find(req.header_fields.at("Host")) == virtual_server.end())
		return &virtual_server.begin()->second;
	return &virtual_server.find(req.header_fields.at("Host"))->second;
}
