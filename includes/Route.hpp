#ifndef ROUTE_HPP
# define ROUTE_HPP

#include "Parser.hpp"
#include "Type.hpp"
#include <Utils.hpp>
#include <arpa/inet.h>
// #include <Ressource.hpp>
#include <Socket.hpp>

// class Ressource;
class Connexion;

class Route
{
public:
	Route(const t_attributes = t_attributes());
	Route(const Route &ref);
	~Route();
	Route &operator=(const Route &rhs);
	// void			handle(t_http_message &req,Connexion *connexion) const;
	IOEvent 			checkRequest(const t_http_message &req, Connexion *Connexion) const; // let a connexion know if her header is conform to the route
	IOEvent 			setRessource(const t_http_message &req, Connexion *connexion) const;
	std::string 		getError(uint http_error) const;
	size_t 				getMaxBodySize() const;
	void 				printAttributes() const;
	t_attributes const 	&getAttributes() const;
	int 				isCGI(std::string const &path) const;

private:
	t_attributes attributes;
};

#endif
