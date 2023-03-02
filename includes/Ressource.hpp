#ifndef RESSOURCE_HPP
#define RESSOURCE_HPP

#include <Type.hpp>
#include <Socket.hpp>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <string.h>
#include <sys/sendfile.h>

class Router;
class Connexion;

#define READ 0
#define WRITE 1
#define MAX_SIZE_ALLOWED 10000000

class Ressource : public IO
{
public:
	Ressource(Connexion *conn): conn(conn), fd_read(-1), fd_write(-1) {};
	virtual ~Ressource() {};
	virtual IOEvent		read() = 0;
	virtual IOEvent		write() = 0;
	virtual IOEvent		closed() = 0;

protected:
	Connexion			*conn;
	t_fd				fd_read;
	t_fd				fd_write;
};

// File && Directory Classes

class GetStaticFile : public Ressource
{
public:
	GetStaticFile(Connexion *conn, std::string file_path);
	~GetStaticFile();

	virtual IOEvent read() { return IOEvent(); };
	virtual IOEvent write() { return IOEvent(); };
	virtual IOEvent closed();
};

class PostStaticFile : public Ressource
{
public:
	PostStaticFile(Connexion *conn, std::string file_path);
	~PostStaticFile();
	virtual	IOEvent		read(){return IOEvent();};  //added because we are obliged to redefine pure funtions. Doesn't do anything concretely !
	virtual IOEvent		write();
	virtual IOEvent		closed();
};

class DeleteStaticFile : public Ressource
{
public:
	DeleteStaticFile(Connexion *conn, std::string file_path);
	~DeleteStaticFile();

	virtual IOEvent read() { return IOEvent(); };
	virtual IOEvent write() { return IOEvent(); };
	virtual IOEvent closed();
};

class GetDirectory : public Ressource
{
public:
	GetDirectory(Connexion *conn, std::string dir_path);
	~GetDirectory();

	virtual IOEvent read() { return IOEvent(); };
	virtual IOEvent write() { return IOEvent(); };
	virtual IOEvent closed() { return IOEvent(); };
	DIR *get_dir();

protected:
	DIR *dir;
};

// CGI Class

class CGI : public Ressource
{
public:
	CGI(Connexion *conn, t_cgiInfo cgiInfo);
	~CGI();

	virtual IOEvent read();
	virtual IOEvent write();
	virtual IOEvent closed();
};

// Redirect Class

class RedirectRessource : public Ressource
{
public:
	RedirectRessource(Connexion *conn, std::string const &url);
	~RedirectRessource();

	virtual IOEvent read() { return IOEvent(); };
	virtual IOEvent write() { return IOEvent(); };
	virtual IOEvent closed() { return IOEvent(); };
};

#endif
