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
	conn->append_response("HTTP/1.1 200 OK\r\n", 0);
	conn->append_response("Content-Type: text/html\r\n", 0);
	conn->append_response("\r\n", 0);

	// append the body
	conn->append_response("<html>\n", 0);
	conn->append_response("<head>\n", 0);
	conn->append_response("<title>Directory Listing</title>\n", 0);
	conn->append_response("</head>\n", 0);
	conn->append_response("<body>\n", 0);
	conn->append_response("<h1>Directory Listing</h1>\n", 0);
	conn->append_response("<ul>\n", 0);

	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_type == DT_REG)
			conn->append_response("<li><a href=\"./" + std::string(entry->d_name) +
									"\">" + std::string(entry->d_name) + "</a></li>\n", 0);
		else if (entry->d_type == DT_DIR)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			conn->append_response("<li><a href=\"./" + std::string(entry->d_name) +
									"/\">" + std::string(entry->d_name) + "/</a></li>\n", 0);
		}
		else
		{
			conn->setError("Error reading the directory " + dir_path, 500);
			throw std::runtime_error("GetDirectory::GetDirectory() readdir failed.");
		}
	}

	conn->append_response("</ul>\n", 0);
	conn->append_response("</body>\n", 0);
	conn->append_response("</html>\n", 0);
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

DIR	*GetDirectory::get_dir() { return dir; }
