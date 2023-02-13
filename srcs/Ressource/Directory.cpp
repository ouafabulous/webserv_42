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

	conn->append_response("html>\n", 0);
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
			//if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			//	continue;
			conn->append_response("<li><a href=\"./" + std::string(entry->d_name) +
						"/\">" + std::string(entry->d_name) + "/</a></li>\n", 0);
		}
	}
	conn->append_response("</ul>\n", 0);
	conn->append_response("</body>\n", 0);
	conn->append_response("</html>\n", 0);
}

GetDirectory::~GetDirectory() {}
// IOEvent	GetDirectory::read() {}
// IOEvent	GetDirectory::write() {}
IOEvent GetDirectory::closed() {}

// list files and format them into html
// git merge main
// errors or not
// select in vscode and compare.
