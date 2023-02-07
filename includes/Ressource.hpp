#pragma once
#ifndef RESSOURCE_HPP
#define RESSOURCE_HPP

#include <Socket.hpp>

// Abstract Ressource Class

class Ressource : public IO
{
public:
	Ressource(Connexion *conn);
	virtual ~Ressource() {};
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
	virtual t_fd		fdDelete() = 0;

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
	GetStaticFile(Connexion *conn, std::string file_path);
	~GetStaticFile();
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};

class PostStaticFile : public Ressource
{
public:
	PostStaticFile(Connexion *conn, std::string file_path);
	~PostStaticFile();
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};

class DeleteStaticFile : public Ressource
{
public:
	DeleteStaticFile(Connexion *conn, std::string file_path);
	~DeleteStaticFile();
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};

class GetDirectory : public Ressource
{
public:
	GetDirectory(Connexion *conn, std::string dir_path);
	~GetDirectory();
	virtual void		read() = 0;
	virtual void		write() = 0;
	virtual void		closed() = 0;
};


// CGI Class

class CGI : public Ressource
{
public:
	CGI(Connexion *conn, std::string cgi_path);
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
