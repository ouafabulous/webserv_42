#pragma once
#ifndef RESSOURCE_HPP
#define RESSOURCE_HPP

#include <Socket.hpp>

#define READ 0
#define WRITE 1

class Ressource : public IO
{
public:
	Ressource(Connexion *conn): conn(conn), is_EOF(false), fd_read(-1), fd_write(-1) {};
	virtual ~Ressource() {};
	virtual IOEvent		read() = 0;
	virtual IOEvent		write() = 0;
	virtual IOEvent		closed() = 0;

protected:
	t_fd				fd_read;
	t_fd				fd_write;
	Connexion			*conn;
	bool				is_EOF;
};


// File && Directory Classes

class GetStaticFile : public Ressource
{
public:
	GetStaticFile(Connexion *conn, std::string file_path);
	~GetStaticFile();
	virtual IOEvent		read();
	virtual IOEvent		closed();
};

class PostStaticFile : public Ressource
{
public:
	PostStaticFile(Connexion *conn, std::string file_path);
	~PostStaticFile();
	virtual IOEvent		write();
	virtual IOEvent		closed();
};

class DeleteStaticFile : public Ressource
{
public:
	DeleteStaticFile(Connexion *conn, std::string file_path);
	~DeleteStaticFile();
	virtual IOEvent		closed();
};

class GetDirectory : public Ressource
{
public:
	GetDirectory(Connexion *conn, std::string dir_path);
	~GetDirectory();
	virtual IOEvent		read();
	virtual IOEvent		write();
	virtual IOEvent		closed();
};


// CGI Class

class CGI : public Ressource
{
public:
	CGI(Connexion *conn, std::string file_path, std::string cgi_path);
	~CGI();
	virtual IOEvent		read();
	virtual IOEvent		write();
	virtual IOEvent		closed();
};


// Redirect Class

class RedirectRessource : public Ressource
{
public:
	RedirectRessource(Connexion *conn, std::string url);
	~RedirectRessource();
	virtual IOEvent		read();
	virtual IOEvent		write();
	virtual IOEvent		closed();
};

class ErrorRessource : public Ressource
{
public:
	ErrorRessource(Connexion *conn, std::string error_response);
	~ErrorRessource();
};

#endif
