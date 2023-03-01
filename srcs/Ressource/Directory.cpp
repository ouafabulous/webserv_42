#include <Ressource.hpp>

GetDirectory::GetDirectory(Connexion *conn, std::string dir_path) : Ressource(conn),
																	dir(NULL)
{
	struct dirent *entry;

	dir = opendir(dir_path.c_str());
	if (dir == NULL)
	{
		conn->setError("Error opening the directory " + dir_path, 404);
		throw std::runtime_error("GetDirectory::GetDirectory() opendir failed.");
	}

	// append the header
	conn->pushResponse("HTTP/1.1 200 OK\r\n");
	conn->pushResponse("Content-Type: text/html\r\n");
	conn->pushResponse("\r\n");

	// append the body
	conn->pushResponse("<html>\n");
	conn->pushResponse("<head>\n");
	conn->pushResponse("<title>Directory Listing</title>\n");
	conn->pushResponse("</head>\n");
	conn->pushResponse("<body>\n");
	conn->pushResponse("<h1>Directory Listing</h1>\n");
	conn->pushResponse("<ul>\n");

	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_type == DT_REG)
			conn->pushResponse("<li><a href=\"./" + std::string(entry->d_name) + "\">" + std::string(entry->d_name) + "</a></li>\n");
		else if (entry->d_type == DT_DIR)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			conn->pushResponse("<li><a href=\"./" + std::string(entry->d_name) + "/\">" + std::string(entry->d_name) + "/</a></li>\n");
		}
		else
		{
			conn->setError("Error reading the directory " + dir_path, 500);
			throw std::runtime_error("GetDirectory::GetDirectory() readdir failed.");
		}
	}

	conn->pushResponse("</ul>\n");
	conn->pushResponse("</body>\n");
	conn->pushResponse("</html>\n");
	conn->setRespEnd();
}

GetDirectory::~GetDirectory()
{
	if (dir)
	{
		if (closedir(dir) == -1)
		{
			conn->setError("Error closing the directory ", 500);
			throw std::runtime_error("GetDirectory::GetDirectory() closedir failed.");
		}
	}
}

// IOEvent GetDirectory::closed() {}

DIR	*GetDirectory::get_dir() { return dir; }
