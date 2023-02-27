#include <Router.hpp>
#include <Ressource.hpp>
#include "Type.hpp"

class CGI;

Route::Route(const t_attributes attributes)
	: attributes(attributes)
{
}
Route::Route(const Route &from)
	: attributes(from.attributes) {}
Route &Route::operator=(const Route &rhs)
{
	// self-assignment guard
	if (this == &rhs)
		return *this;
	attributes = rhs.attributes;
	return *this;
}
Route::~Route()
{
}

std::string Route::getError(uint http_error) const
{
	return attributes.error_files.getError(http_error);
}

size_t Route::getMaxBodySize() const
{
	return attributes.max_body_length;
}
void Route::printAttributes() const
{
	std::cout << "-----Port-----" << std::endl;
	std::cout << attributes.port << std::endl;
	std::cout << "-----Methods allowed-----" << std::endl;
	if (attributes.allowed_methods & GET)
	{
		std::cout << "GET ";
	}
	else if (attributes.allowed_methods & POST)
	{
		std::cout << "POST ";
	}
	else if (attributes.allowed_methods & DELETE)
	{
		std::cout << "DELETE";
	}
	std::cout << std::endl;
	std::cout << "-----Server Names-----" << std::endl;
	std::vector<std::string>::const_iterator str_iter;
	for (str_iter = attributes.server_name.begin(); str_iter != attributes.server_name.end(); ++str_iter)
	{
		std::cout << *str_iter << " ";
	}
	std::cout << std::endl;

	std::cout << "Location: \t\t" << attributes.location << std::endl;

	std::cout << "Max body length: \t" << attributes.max_body_length << std::endl;

	std::cout << "Root: \t\t\t" << attributes.root << std::endl;

	std::cout << "Methods allowed: \t";
	if (attributes.allowed_methods & GET) {
		std::cout << "GET ";
	}
	if (attributes.allowed_methods & POST) {
		std::cout << "POST ";
	}
	if (attributes.allowed_methods & DELETE){
		std::cout << "DELETE";
	}
	std::cout << std::endl;

	std::cout << "Directory listing: \t" << attributes.directory_listing << std::endl;

	std::cout << "CGI setup: \t\t";
	for (std::map<std::string, std::string>::const_iterator it = attributes.cgi_path.begin(); it != attributes.cgi_path.end(); ++it){
		std::cout << it->first << "->" << it->second << " ";
	}
	std::cout << std::endl;

	std::cout << "Redirect: \t" << attributes.redirect << std::endl;
}

IOEvent Route::checkRequest(const t_http_message &req, Connexion *conn) const
{
	t_methods request_method = req.request_line.method;
	if (attributes.allowed_methods & request_method)
	{
		// je sais pas trop comment utiliser le debugge ici
		return conn->setError("", 400);
		// return 1;
	}
	return IOEvent(); 
}

int Route::isCGI(std::string const &path) const
{
	std::map<std::string, std::string>::const_iterator cgiIt;
	int i = 0;
	for (cgiIt = attributes.cgiMap.begin(); cgiIt != attributes.cgiMap.end(); cgiIt++)
	{
		if (containsSubstring(path, cgiIt->first))
		{
			return (i);
		}
		i++;
	}
	return (-1);
}

IOEvent Route::setRessource(const t_http_message &req, Connexion *conn) const
{
	t_request_line	reqLine = req.request_line;
	std::string		completePath = attributes.root + reqLine.path;
	
	if (attributes.redirect.length())
	{

		conn->setRessource(new RedirectRessource(conn, attributes.redirect));
		return (IOEvent());
	}
	
	int cgi = isCGI(completePath);
	if (cgi > -1)
	{
		if (fileExists(extractBeforeChar(completePath, '?').c_str()))
		{
			if (checkPermissions(completePath, S_IXUSR | S_IXGRP))
			{
				std::map<std::string, std::string>::const_iterator	cgiMapIter = attributes.cgiMap.begin();
				//iterating through the CGI paths map in order to find the right path of our executanle;
				for (int i = 0; i != cgi; i++){
					cgiMapIter++;
				}
				t_cgiInfo cgiInfo(extractBeforeChar(completePath, '?'), extractAfterChar(completePath, '?'), cgiMapIter->second);
				conn->setRessource(new CGI(conn, cgiInfo));
				return (IOEvent());
			}
			else
			{
				return (conn->setError("", 403));
			}
		}
	}
	if (fileExists(completePath.c_str())){
		if (reqLine.method & GET)
		{
			if (checkPermissions(completePath, S_IRUSR | S_IRGRP))
			{
				conn->setRessource(new GetStaticFile(conn, completePath));
				return (IOEvent());
			}
			else
			{
				return (conn->setError("", 403));
			}
		}
		else if (reqLine.method & POST)
		{
			conn->setRessource(new PostStaticFile(conn, completePath));
			return (IOEvent());
		}
	}
	else if (directoryExists(completePath.c_str()) && reqLine.method & GET)
	{
		if (checkPermissions(completePath, S_IRUSR | S_IRGRP))
		{
			if (fileExists((completePath + "index.html").c_str()))
			{
				if (checkPermissions(completePath + "index.html", S_IRUSR | S_IRGRP)) {
					conn->setRessource(new GetStaticFile(conn, completePath));
					return (IOEvent());
				}
			}
			else if (attributes.directory_listing)
			{
				if (checkPermissions(completePath, S_IXUSR | S_IXGRP)) {
					conn->setRessource(new GetDirectory(conn, completePath)); 
					return (IOEvent());
				}
			}
		}
		else
		{
				return (conn->setError("", 403));
		}
	}
	return (conn->setError("", 404));
}