#pragma once
#ifndef RESSOURCE
# define RESSOURCE

#include "Socket.hpp"

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

class RedirectRessource : public Ressource {
public:
  RedirectRessource(Connexion *conn, std::string url);
  ~RedirectRessource();
  void handle(Connexion &conn);
};

#endif
