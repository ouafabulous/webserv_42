#pragma once
#ifndef ROUTER_HPP
# define ROUTER_HPP

#include "Server.hpp"
#include "Socket.hpp"
#include "Ressource.hpp"

class Router {
public:
  typedef std::map<t_network_address, std::map<std::string, Route>> router_map;
  router_map my_map;
  Router(std::string &config);
  void setRoute(Connexion &conn);
  bool checkHeader(t_http_message &msg);
  std::vector<t_network_address> getAddr();
};

class Route {
public:
  typedef char t_methods;
  t_methods methods_allowed;
  size_t max_body_length;
  std::vector<std::string> server_names;
  std::map<uint,std::string> errors;
  std::string redirect;
  std::string static_file;
  bool directory_listing;
  void handle(Connexion &conn);
  Ressource* create_ressource(Connexion &conn);
};

#endif
