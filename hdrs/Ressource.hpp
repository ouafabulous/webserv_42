#pragma once
#ifndef RESSOURCE_HPP
#define RESSOURCE_HPP

#include "Socket.hpp"

class Ressource
{
public:
	Ressource(Connexion *conn);
	virtual ~Ressource() = 0;
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
	virtual t_fd		fd_delete();

protected:
	int					fd_read;
	int					fd_write;
	Connexion			*conn;
	std::vector<char>	response;
	bool				rEOF;
};

class GetStaticFile : public Ressource
{
public:
	GetStaticFile(Connexion *conn);
	~GetStaticFile();
	void handle(Connexion &conn);
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};

class PostStaticFile : public Ressource
{
public:
	PostStaticFile(Connexion *conn);
	~PostStaticFile();
	void handle(Connexion &conn);
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};

class DeleteStaticFile : public Ressource
{
public:
	DeleteStaticFile(Connexion *conn);
	~DeleteStaticFile();
	void handle(Connexion &conn);
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};

class CGI : public Ressource
{
public:
	CGI(Connexion *conn);
	~CGI();
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};

class RedirectRessource : public Ressource
{
public:
	RedirectRessource(Connexion *conn, std::string url);
	~RedirectRessource();
	void handle(Connexion &conn);
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};

#endif