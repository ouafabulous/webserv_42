#pragma once
#ifndef RESSOURCE_HPP
#define RESSOURCE_HPP

#include <Socket.hpp>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <string.h>
#include <sys/sendfile.h>

#define READ 0
#define WRITE 1

const	std::map<std::string, std::string> MIME_MAP =
{
	{".html", "text/html"},
	{".htm", "text/html"},
	{".css", "text/css"},
	{".js", "application/javascript"},
	{".jpg", "image/jpeg"},
	{".jpeg", "image/jpeg"},
	{".png", "image/png"},
	{".gif", "image/gif"},
	{".svg", "image/svg+xml"},
	{".txt", "text/plain"},
	{".pdf", "application/pdf"},
	{".zip", "application/zip"},
	{".gz", "application/gzip"},
	{".tar", "application/x-tar"},
	{".mp3", "audio/mpeg"},
	{".mp4", "video/mp4"},
	{".mpeg", "video/mpeg"},
	{".mpg", "video/mpeg"},
	{".avi", "video/x-msvideo"},
	{".wmv", "video/x-ms-wmv"},
	{".mov", "video/quicktime"},
	{".flv", "video/x-flv"},
	{".swf", "application/x-shockwave-flash"},
	{".webm", "video/webm"},
	{".ogg", "video/ogg"},
	{".ogv", "video/ogg"},
	{".oga", "audio/ogg"},
	{".ogx", "application/ogg"},
	{".aac", "audio/aac"},
	{".wav", "audio/wav"},
	{".webp", "image/webp"},
	{".ico", "image/x-icon"},
	{".xml", "application/xml"},
	{".json", "application/json"}
};

const	std::string			get_mime(std::string &file_path)
{
	size_t pos = file_path.find_last_of('.');
	if (pos == std::string::npos)
		return "application/octet-stream";

	std::string extension = file_path.substr(pos);
	if (MIME_MAP.count(extension) == 0)
		return "application/octet-stream";
	return MIME_MAP.at(extension);
};

class Ressource : public IO
{
public:
	Ressource(Connexion *conn): conn(conn), is_EOF(false), fd_read(-1), fd_write(-1) {};
	virtual ~Ressource() {};
	virtual IOEvent		read() = 0;
	virtual IOEvent		write() = 0;
	virtual IOEvent		closed() = 0;

protected:
	Connexion			*conn;
	bool				is_EOF;
	t_fd				fd_read;
	t_fd				fd_write;
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
	virtual IOEvent		closed();
	DIR					*get_dir();

protected:
	DIR					*dir;
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
