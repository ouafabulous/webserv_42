#pragma once
#ifndef RESSOURCE_HPP
#define RESSOURCE_HPP

#include <Socket.hpp>

// Abstract Ressource Class

class Ressource
{
public:
	Ressource(Connexion *conn);
	virtual ~Ressource() = 0;
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
	virtual t_fd		fdDelete();

protected:
	const t_fd			fd_read;
	const t_fd			fd_write;
	Connexion			*conn;
	std::vector<char>	response;
	bool				is_EOF;
};


// File && Directory Classes

class GetStaticFile : public Ressource
{
public:
	GetStaticFile(Connexion *conn);
	~GetStaticFile();
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};

class PostStaticFile : public Ressource
{
public:
	PostStaticFile(Connexion *conn);
	~PostStaticFile();
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};

class DeleteStaticFile : public Ressource
{
public:
	DeleteStaticFile(Connexion *conn);
	~DeleteStaticFile();
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};

class GetDirectory : public Ressource
{
public:
	GetDirectory(Connexion *conn);
	~GetDirectory();
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};


// CGI Class

class CGI : public Ressource
{
public:
	CGI(Connexion *conn);
	~CGI();
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};


// Redirect Class

class RedirectRessource : public Ressource
{
public:
	RedirectRessource(Connexion *conn, std::string url);
	~RedirectRessource();
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};

#endif
