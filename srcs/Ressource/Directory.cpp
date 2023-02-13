#include <Ressource.hpp>
#include <dirent.h>

std::vector<std::string> files;

GetDirectory::GetDirectory(Connexion *conn, std::string dir_path) : Ressource(conn)
{
}

GetDirectory::~GetDirectory() {}
// IOEvent	GetDirectory::read() {}
// IOEvent	GetDirectory::write() {}
IOEvent GetDirectory::closed() {}

// list files and format them into html
// git merge main
// errors or not
// select in vscode and compare.
