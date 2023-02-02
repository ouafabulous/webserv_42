#pragma once
#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "Server.hpp"

class ListenSocket : public IO {
public:
  int socket;
  t_network_address netAddr;
  ListenSocket(t_network_address netAddr);
  virtual void read();
  virtual void write();
  virtual void closed();
  virtual int fd_delete();
};

class Connexion : public IO {
public:
  int socket;
  t_network_address netAddr;
  ListenSocket* listen_socket;
  t_http_message request;
  std::string request_header;
  bool header_end;
  const Router& router;
  Route* route;
  Ressource* ressource;
  Connexion(t_network_address netAddr, int socket, Router& router);
  virtual void read();
  virtual void write();
  void closed();
  void read_header();
  bool read_body();
  void get_header();
  void parse_header();
  virtual int fd_delete();
};

#endif
