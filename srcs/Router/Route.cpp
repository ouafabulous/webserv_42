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
	std::cout << "-----Location-----" << std::endl;
	std::cout << attributes.location << std::endl;
	std::cout << "-----Max body length-----" << std::endl;
	std::cout << attributes.max_body_length << std::endl;
	std::cout << "-----Redirect-----" << std::endl;
	std::cout << attributes.redirect << std::endl;
	std::cout << "-----Root-----" << std::endl;
	std::cout << attributes.root << std::endl;
	std::cout << "-----Directory listing-----" << std::endl;
	std::cout << attributes.directory_listing << std::endl;
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

std::string findCgiExecPath(std::map<std::string, std::string> const &cgiPathMap, int cgiIndex)
{
	std::map<std::string, std::string>::const_iterator cgiMapIter = cgiPathMap.begin();
	// iterating through the CGI paths map in order to find the right path of our executanle;
	for (int i = 0; i != cgiIndex; i++)
	{
		cgiMapIter++;
	}
	return (cgiMapIter->second);
}

IOEvent Route::setRessource(const t_http_message &req, Connexion *conn) const
{
	t_request_line reqLine = req.request_line;
	std::string completePath = attributes.root + reqLine.path;

//1- redirect-handling 
	if (!attributes.redirect.empty())
	{
		conn->setRessource(new RedirectRessource(conn, attributes.redirect));
		return (IOEvent());
	}

//2- CGI handling
	const int cgiIndex = isCGI(completePath);
	if (cgiIndex >= 0)
	{
		const std::string &execPath = findCgiExecPath(attributes.cgiMap, cgiIndex);
		if (checkPermissions(completePath, S_IXUSR | S_IXGRP))
		{
			const t_cgiInfo cgiInfo(
				extractBeforeChar(completePath, '?'),
				extractAfterChar(completePath, '?'),
				execPath);
			try
			{
				conn->setRessource(new CGI(conn, cgiInfo));
				return (IOEvent());
			}
			catch (const std::runtime_error &e)
			{
				return IOEvent(FAIL, conn, e.what(), 500);
			}
		}
		else
		{
			return conn->setError("", 403);
		}
	}

//3- File handling
	if (fileExists(completePath.c_str()))
	{
		switch (reqLine.method)
		{
		case GET:
			conn->setRessource(new GetStaticFile(conn, completePath));
			break;
		case POST:
			conn->setRessource(new PostStaticFile(conn, completePath));
			break;
		case DELETE:
			conn->setRessource(new DeleteStaticFile(conn, completePath));
			break;
		default:
			return conn->setError("", 405);
		}
		return (IOEvent());
	}

//4- Directory handling
	if (directoryExists(completePath.c_str()))
	{
		if (reqLine.method == GET)
		{
			const std::string indexPath = completePath + "index.html";
			if (fileExists(indexPath.c_str()))
			{
				if (checkPermissions(indexPath, R_OK))
				{
					try
					{
						conn->setRessource(new GetStaticFile(conn, indexPath));
						return (IOEvent());
					}
					catch (const std::runtime_error &e)
					{
						return IOEvent(FAIL, conn, e.what(), 500);
					}
				}
				else
				{
					return conn->setError("", 403);
				}
			}
			else if (attributes.directory_listing && checkPermissions(completePath, X_OK))
			{
				try
				{
					conn->setRessource(new GetDirectory(conn, completePath));
					return (IOEvent());
				}
				catch (const std::runtime_error &e)
				{
					return IOEvent(FAIL, conn, e.what(), 500);
				}
			}
		}
		else if ((reqLine.method & POST) || (reqLine.method & DELETE))
		{
			return conn->setError("", 405);
		}
	}

//5- other
	return conn->setError("", 404);
}