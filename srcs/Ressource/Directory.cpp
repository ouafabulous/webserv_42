#include <Ressource.hpp>

GetDirectory::GetDirectory(Connexion *conn, std::string dir_path) :	conn(conn),
																	fd_read(-1),
																	fd_write(-1)
																	is_EOF(false)
{
}

GetDirectory::~GetDirectory() {}
IOEvent	GetDirectory::read() {}
IOEvent	GetDirectory::write() {}
IOEvent	GetDirectory::closed() {}
