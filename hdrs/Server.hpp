#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <netinet/in.h>
#include "Router.hpp"

struct t_request_line {
  std::string 							method;
  std::string 							path;
  std::string 							http_version;
};

struct t_http_message {
  t_request_line						request_line;
  std::map<std::string, std::string>	header_fields;
  std::vector<char> 					body;
};

struct t_network_address {
  in_addr_t address;
  in_port_t port;
};

class Server {
public:
  typedef int t_fd;
  Server(std::string confFile);
  static std::map<t_fd, IO*> socks;
  static int epollfd;
  Router router;
  void routine();
};


class IO {
public:
  IO();
  virtual void read() = 0;
  virtual void write() = 0;
  virtual void closed() = 0;
  virtual int fd_delete() = 0;
};

#endif
