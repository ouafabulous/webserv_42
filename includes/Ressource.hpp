#ifndef RESSOURCE_HPP
#define RESSOURCE_HPP

#include <Type.hpp>
#include <Socket.hpp>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <string.h>
#include <sys/sendfile.h>
#include <signal.h>
#include <Route.hpp>
#include <sstream>

class Router;
class Connexion;

#define READ 0
#define WRITE 1
#define MAX_SIZE_ALLOWED 10000000

class Ressource : public IO
{
public:
	Ressource(Connexion *conn);
	virtual ~Ressource();
	virtual IOEvent read();
	virtual IOEvent write();
	IOEvent closed();

protected:
	Connexion *conn;
	t_fd fd_read;
	t_fd fd_write;
};

// File && Directory Classes

class GetStaticFile : public Ressource
{
public:
	GetStaticFile(Connexion *conn, std::string file_path);
	~GetStaticFile();
};

class PostStaticFile : public Ressource
{
public:
	PostStaticFile(Connexion *conn, std::string file_path);
	~PostStaticFile();
};

class DeleteStaticFile : public Ressource
{
public:
	DeleteStaticFile(Connexion *conn, std::string file_path);
	~DeleteStaticFile();
};

class GetDirectory : public Ressource
{
public:
	GetDirectory(Connexion *conn, std::string dir_path);
	~GetDirectory();

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
	IOEvent read();
	IOEvent write();

private:
	pid_t pid;
	std::string raw_header;
	std::string	raw_body;
	int content_length;
	bool header_received;
	bool read_header();
};

// Redirect Class

class RedirectRessource : public Ressource
{
public:
	RedirectRessource(Connexion *conn, std::string const &url);
	~RedirectRessource();
};

class GetError : public Ressource
{
public:
	GetError(Connexion *conn, uint error_no, std::string file_path);
	~GetError();
private:
	bool	is_custom;
	static const std::string&	getDefaultError(uint status_code);
};

#endif
