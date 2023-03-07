
#include <iostream>
#include <set>
#include <Type.hpp>
#include "Router.hpp"


// int i = 0;
Router::Router(Parser const &confile)
{
	// BlockServer 	*curr_vserver = confile.getBlock();
	std::vector<BlockServer>::const_iterator curr_vserver;
	for (curr_vserver = confile.getBlockServers().begin(); curr_vserver != confile.getBlockServers().end(); curr_vserver++)
	{

		t_attributes 	attributes;
		fillAttributes(&attributes, curr_vserver->getDirectives());
		std::vector<BlockLocation>::const_iterator it;
		std::vector<BlockLocation> const &locations = curr_vserver->getLocations();
		t_vserver		vserver(attributes.server_name);
		// push_back default virtual_server route
		attributes.location = "";
		vserver._routes.push_back(Route(attributes));
		// push_back each location route
		for (it = locations.begin(); it != locations.end(); it++)
		{
			attributes.location = it->getLocationValue();
			fillAttributes(&attributes, it->getDirectives());
			vserver._routes.push_back(Route(attributes));
		}
		_network_map[t_network_address(INADDR_ANY, htons(attributes.port))].push_back(vserver);
	}
	printRoutes();
}

Router::~Router()
{
}

void Router::printRoutes() const
{
	for (t_network_map::const_iterator it1 = _network_map.begin(); it1 != _network_map.end(); it1++)
	{
		t_vserver_vec::const_iterator	vserverIt;
		for (vserverIt = it1->second.begin(); vserverIt != it1->second.end(); ++vserverIt)
		{
			for (std::vector<Route>::const_iterator routeIt = vserverIt->_routes.begin(); routeIt != vserverIt->_routes.end(); routeIt++)
			{
				std::cout << "------------------------------" <<  std::endl;
				std::cout << "---Adresse: " << it1->first.second << std::endl;
				std::cout << std::endl;
				std::cout << "---Route: ";
				std::cout << std::endl;
				routeIt->printAttributes();
				std::cout << "--------------\n\n" << std::endl;
			}
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
		if (it->getDirectiveName() == ALLOWEDMETHODS)
		{
			if (it->getDirectiveValues().size())
				attributes->allowed_methods = NO_METHOD;
			for (directiveValIter = (it->getDirectiveValues()).begin(); directiveValIter != (it->getDirectiveValues()).end(); directiveValIter++){
				attributes->allowed_methods |= methodsDict[directiveValIter->_stringValue]; //we are supposing here that we can get only one allowed_method at once, while in reality we can have up to 3 and then I gotta iterate all over them
			}
		}
		else if (it->getDirectiveName() == LISTEN)
		{
			attributes->port = static_cast<uint>((it->getDirectiveValues())[0]._intValue);
		}
		else if (it->getDirectiveName() == SERVERNAMES)
		{
			for (directiveValIter = (it->getDirectiveValues()).begin(); directiveValIter != (it->getDirectiveValues()).end(); directiveValIter++){
				attributes->server_name.push_back(directiveValIter->_stringValue);
			}
		}
		else if (it->getDirectiveName() == MAXBODYSIZE)
		{
			attributes->max_body_length = static_cast<size_t>((it->getDirectiveValues())[0]._intValue);
		}
		else if (it->getDirectiveName() == REDIRECT)
		{
			attributes->redirect = (it->getDirectiveValues())[0]._stringValue;
		}
		else if (it->getDirectiveName() == ROOT)
		{
			attributes->root = (it->getDirectiveValues())[0]._stringValue;
		}
		else if (it->getDirectiveName() == AUTOINDEX)
		{
			((it->getDirectiveValues())[0]._stringValue == std::string("on")) ? attributes->directory_listing = true : attributes->directory_listing = false;
		}
		else if (it->getDirectiveName() == CGISETUP) {
			attributes->cgi_path[(it->getDirectiveValues())[0]._stringValue] = (it->getDirectiveValues())[1]._stringValue;
		}
		else if (it->getDirectiveName() == ERRORFILE) {
			attributes->error_files.addError(stringToInt((it->getDirectiveValues())[0]._stringValue), (it->getDirectiveValues())[1]._stringValue);
		}
		else if (it->getDirectiveName() == INDEX)
		{
			attributes->index = (it->getDirectiveValues())[0]._stringValue;
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

const t_vserver&	Router::findVserver(const t_network_address addr, const std::string& host) const {
	const t_vserver_vec&			vserver_vec = _network_map.at(addr);

	for (
		t_vserver_vec::const_iterator	vserverIt = vserver_vec.begin();
		vserverIt != vserver_vec.end();
		vserverIt++)
	{
		if (isInServerName(host, vserverIt->_server_names))
			return *vserverIt;
	}
	return *vserver_vec.begin();
}



const Route *Router::getRoute(const t_network_address netAddr, const t_http_message &req) const
{
	const t_vserver&				vserver = findVserver(netAddr, req.header_fields.at("Host"));
	const Route*					response = &(*vserver._routes.begin());
	size_t 							matching_char_result = 0;


	for (std::vector<Route>::const_iterator routeIt = vserver._routes.begin(); routeIt != vserver._routes.end(); routeIt++)
	{
		if (matchingLocation(req.request_line.path, routeIt->getAttributes().location) > matching_char_result) {
			response = &(*routeIt);
			matching_char_result = matchingLocation(req.request_line.path, routeIt->getAttributes().location);
		}
	}
	return (response);
}
