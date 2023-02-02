#include <string>
#include <map>
#include <vector>
#include <netinet/in.h>

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

class IO {
public:
  IO();
  virtual void read() = 0;
  virtual void write() = 0;
  virtual void closed() = 0;
  virtual int fd_delete() = 0;
};

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

class Ressource {
public:
  Ressource(Connexion *conn);
  virtual ~Ressource() = 0;
  virtual void handle(Connexion &conn) = 0;
};

class StaticFileRessource : public Ressource {
public:
  StaticFileRessource(Connexion *conn);
  ~StaticFileRessource();
  void handle(Connexion &conn);
};

class DynamicRessource : public Ressource {
public:
  DynamicRessource(Connexion *conn);
  ~DynamicRessource();
  void handle(Connexion &conn);
};

class ErrorRessource : public Ressource {
public:
  ErrorRessource(Connexion *conn, uint code);
  ~ErrorRessource();
  void handle(Connexion &conn);
};

class RedirectRessource : public Ressource {
public:
  RedirectRessource(Connexion *conn, std::string url);
  ~RedirectRessource();
  void handle(Connexion &conn);
};
