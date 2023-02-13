#include <Ressource.hpp>
#include <dirent.h>
#include <cstring>

std::vector<std::string> files;

GetDirectory::GetDirectory(Connexion *conn, std::string dir_path) : Ressource(conn)
{
	DIR	*dir;
	struct dirent *entry;
	std::string directory = dir_path;

	dir = opendir(directory.c_str());
	if (dir == NULL)
		throw std::runtime_error("GetDirectory::GetDirectory() opendir failed.");

	response.append("html>\n");
	response.append("<head>\n");
	response.append("<title>Directory Listing</title>\n");
	response.append("</head>\n");
	response.append("<body>\n");
	response.append("<h1>Directory Listing</h1>\n");
	response.append("<ul>\n");

	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_type == DT_REG)
			response.append("<li><a href=\"./" + std::string(entry->d_name) +
						"\">" + std::string(entry->d_name) + "</a></li>\n");
		else if (entry->d_type == DT_DIR)
		{
			//if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			//	continue;
			response.append("<li><a href=\"./" + std::string(entry->d_name) +
						"/\">" + std::string(entry->d_name) + "/</a></li>\n");
		}
	}
	response.append("</ul>\n");
	response.append("</body>\n");
	response.append("</html>\n");
}

GetDirectory::~GetDirectory() {}
// IOEvent	GetDirectory::read() {}
// IOEvent	GetDirectory::write() {}
IOEvent GetDirectory::closed() {}

// list files and format them into html
// git merge main
// errors or not
// select in vscode and compare.
