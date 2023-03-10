#include <Ressource.hpp>

GetDirectory::GetDirectory(Connexion *conn, std::string dir_path) : Ressource(conn),
																	dir(NULL)
{
	Logger::info << conn->client_ip_addr << " - list directory " << dir_path << std::endl;

	struct dirent *entry;

	dir = opendir(dir_path.c_str());
	if (dir == NULL)
	{
		conn->setError("Error opening the directory " + dir_path, 404);
		throw std::runtime_error("GetDirectory::GetDirectory() opendir failed.");
	}

	std::string body = "<html>\n";
	body +="<head>\n";
	body +="<title>Directory Listing</title>\n";
	body +="</head>\n";
	body +="<body>\n";
	body +="<h1>Directory Listing</h1>\n";
	body +="<ul>\n";
	body += "<li><a href=\"../\">..</a></li>\n";

	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_type == DT_REG)
			body += "<li><a href=\"./" + std::string(entry->d_name) + "\">" + std::string(entry->d_name) + "</a></li>\n";
		else if (entry->d_type == DT_DIR)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			body += "<li><a href=\"./" + std::string(entry->d_name) + "/\">" + std::string(entry->d_name) + "/</a></li>\n";
		}
		else
		{
			conn->setError("Error reading the directory " + dir_path, 500);
			throw std::runtime_error("GetDirectory::GetDirectory() readdir failed.");
		}
	}

	body +="</ul>\n";
	body +="</body>\n";
	body +="</html>\n";

	conn->pushResponse(http_header_formatter(200, body.length(), "text/html"));
	conn->pushResponse(body);

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

DIR	*GetDirectory::get_dir() { return dir; }
