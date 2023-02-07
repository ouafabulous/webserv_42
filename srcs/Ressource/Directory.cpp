#include <Ressource.hpp>

GetDirectory::GetDirectory(Connexion *conn, std::string dir_path) :	conn(conn),
																	fd_read(-1),
																	fd_write(-1)
																	response(),
																	is_EOF(false)
{
}

GetDirectory::~GetDirectory() {}
void	GetDirectory::read() {}
void	GetDirectory::write() {}
void	GetDirectory::closed() {}
