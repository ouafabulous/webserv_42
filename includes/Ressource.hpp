#pragma once
#ifndef RESSOURCE_HPP
#define RESSOURCE_HPP

#include <Router.hpp>
#include <Socket.hpp>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <string.h>
#include <sys/sendfile.h>

#define READ 0
#define WRITE 1
#define MAX_SIZE_ALLOWED 10000000

typedef std::map<std::string, std::string> MimeMap;

const MimeMap &get_mime_map()
{
	static MimeMap mime_map;

	if (mime_map.empty())
	{
		mime_map[".html"] = "text/html";
		mime_map[".htm"] = "text/html";
		mime_map[".css"] = "text/css";
		mime_map[".js"] = "application/javascript";
		mime_map[".jpg"] = "image/jpeg";
		mime_map[".jpeg"] = "image/jpeg";
		mime_map[".png"] = "image/png";
		mime_map[".gif"] = "image/gif";
		mime_map[".svg"] = "image/svg+xml";
		mime_map[".txt"] = "text/plain";
		mime_map[".pdf"] = "application/pdf";
		mime_map[".zip"] = "application/zip";
		mime_map[".gz"] = "application/gzip";
		mime_map[".tar"] = "application/x-tar";
		mime_map[".mp3"] = "audio/mpeg";
		mime_map[".mp4"] = "video/mp4";
		mime_map[".mpeg"] = "video/mpeg";
		mime_map[".mpg"] = "video/mpeg";
		mime_map[".avi"] = "video/x-msvideo";
		mime_map[".wmv"] = "video/x-ms-wmv";
		mime_map[".mov"] = "video/quicktime";
		mime_map[".flv"] = "video/x-flv";
		mime_map[".swf"] = "application/x-shockwave-flash";
		mime_map[".webm"] = "video/webm";
		mime_map[".ogg"] = "video/ogg";
		mime_map[".ogv"] = "video/ogg";
		mime_map[".oga"] = "audio/ogg";
		mime_map[".ogx"] = "application/ogg";
		mime_map[".aac"] = "audio/aac";
		mime_map[".wav"] = "audio/wav";
		mime_map[".webp"] = "image/webp";
		mime_map[".ico"] = "image/x-icon";
		mime_map[".xml"] = "application/xml";
		mime_map[".json"] = "application/json";
	}
	return mime_map;
}

const	std::string			get_mime(const std::string &file_path)
{
	size_t pos = file_path.find_last_of('.');
	if (pos == std::string::npos)
		return "application/octet-stream; charset=utf-8";

	std::string extension = file_path.substr(pos);
	const MimeMap &mimeMap = get_mime_map();
	MimeMap::const_iterator it = mimeMap.find(extension);
	if (it == mimeMap.end())
		return "application/octet-stream; charset=utf-8";
	return it->second;
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

private:
	size_t				bytes_read;
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
private:
	size_t				bytes_read;
};

// Redirect Class

class RedirectRessource : public Ressource
{
public:
	RedirectRessource(Connexion *conn, const std::string url);
	~RedirectRessource();
};

#endif
