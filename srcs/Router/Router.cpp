
#include <iostream>
#include <set>
#include "Type.hpp"
#include "Router.hpp"


// int i = 0;
Router::Router(Parser const &confile)
{
	BlockServer 	*tmp1 = confile.getBlock();
	while (tmp1)
	{
		t_attributes 	attributes = {};
		fillAttributes(&attributes, tmp1->getDirectives());
		std::vector<BlockLocation *>::const_iterator it;
		std::vector<BlockLocation *> const &locations = tmp1->getChilds();
		Route	globalRoute(attributes);
		NetworkRoute	globalNetworkRoute(t_network_address(INADDR_ANY, htons(attributes.port)), attributes.server_name, globalRoute);
		_network_map[t_network_address(INADDR_ANY, htons(attributes.port))].push_back(globalNetworkRoute);
		for (it = locations.begin(); it != locations.end(); it++)
		{
			attributes.location = (*it)->getLocationValue();
			fillAttributes(&attributes, (*it)->getDirectives());
			Route			route(attributes);
			NetworkRoute	networkRoute(t_network_address(INADDR_ANY, htons(attributes.port)), attributes.server_name, globalRoute);
			_network_map[t_network_address(INADDR_ANY, htons(attributes.port))].push_back(networkRoute);
		}

		tmp1 = tmp1->getSibling();
	}
	// printRoutes();
}

Router::~Router()
{
}

void Router::printRoutes() const
{
	for (t_network_map::const_iterator it1 = _network_map.begin(); it1 != _network_map.end(); it1++)
	{
		t_route_vector::const_iterator	iterRouter;
		for (iterRouter = it1->second.begin(); iterRouter != it1->second.end(); ++iterRouter)
		{
				std::cout << "------------------------------" <<  std::endl;
				std::cout << "---Adresse: " << iterRouter->_address << std::endl;
				std::cout << std::endl;
				std::cout << "---Route: ";
				std::cout << std::endl;
				iterRouter->_route.printAttributes();
			std::cout << "--------------\n\n" << std::endl;
		}
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
			attributes->root = (it->getDirectiveValues())[0]._stringValue;
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
	for (t_network_map::const_iterator it = _network_map.begin(); it != _network_map.end(); it++)
		response.push_back(it->first);

	return (response);
}

bool	Router::isInServerName(const std::string& host, const std::vector<std::string>	server_names) const {
	for (std::vector<std::string>::const_iterator servNameIter = server_names.begin(); servNameIter != server_names.end(); servNameIter++){
		if (*servNameIter == host)
			return true;
	}
	return false;
}

t_attributes const &Route::getAttributes() const{
	return(this->attributes);
}



const Route *Router::getRoute(const t_network_address netAddr, const t_http_message &req) const
{
	const t_route_vector&	route_vector = _network_map.at(netAddr);
	t_route_vector::const_iterator iterRoutes = route_vector.begin();
	t_route_vector::const_iterator response = iterRoutes;
	size_t matching_char_result = 0;

	for (; iterRoutes != route_vector.end(); iterRoutes++){
		if (!isInServerName(req.header_fields.at("Host"), iterRoutes->_server_name))
			continue;
		if (matchingChar(req.request_line.path, iterRoutes->_route.getAttributes().location) > matching_char_result) {
			response = iterRoutes;
			matching_char_result = matchingChar(req.request_line.path, iterRoutes->_route.getAttributes().location);
		}
	}
	return &(response->_route);
}
