#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <cstring>

int main(int ac, char **av)
{
	DIR *dir;
	struct dirent *entry;
	std::string directory = ".";

	if (ac == 2)
		directory = av[1];

	dir = opendir(directory.c_str());
	if (dir == NULL)
	{
		std::cerr << "Can't open directory " << directory << ".\n"
				  << "Error : " << strerror(errno) << std::endl;
		return 1;
	}

	std::ofstream outfile;
	outfile.open("index.html");

	outfile << "<html>\n"
			<< "<head>\n"
			<< "<title>Directory Listing</title>\n"
			<< "</head>\n"
			<< "<body>\n"
			<< "<h1>Directory Listing</h1>\n"
			<< "<ul>\n";

	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_type == DT_REG) // regular file
			outfile << "<li><a href=\"" << entry->d_name
					<< "\">" << entry->d_name << "</a></li>\n";
		else if (entry->d_type == DT_DIR) // directory
		{
			//if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			//	continue;
			outfile << "<li><a href=\"" << entry->d_name
					<< "/\">" << entry->d_name << "/</a></li>\n";
		}
	}

	outfile << "</ul>\n"
			<< "</body>\n"
			<< "</html>\n";
	outfile.close();

	closedir(dir);
	return 0;
}
