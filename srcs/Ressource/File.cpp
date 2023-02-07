#include <Ressource.hpp>

GetStaticFile::GetStaticFile(Connexion *conn, std::string file_path) :	conn(conn),
																		fd_write(-1),
																		fd_read(open(file_path.c_str(), O_RDONLY))
{
}

GetStaticFile::~GetStaticFile() {}
void	GetStaticFile::read() {}
void	GetStaticFile::write() {}
void	GetStaticFile::closed() {}

PostStaticFile::PostStaticFile(Connexion *conn) :	conn(conn),
													fd_read(-1),
													fd_write(recv(conn.socket, conn.request, BUFFER_SIZE, 0))
{
}

PostStaticFile::~PostStaticFile() {}
void	PostStaticFile::read() {}
void	PostStaticFile::write() {}
void	PostStaticFile::closed() {}

DeleteStaticFile::DeleteStaticFile(Connexion *conn) :	conn(conn),
														fd_read(-1),
														fd_write(-1)
{
}

DeleteStaticFile::~DeleteStaticFile() {}
void	DeleteStaticFile::read() {}
void	DeleteStaticFile::write() {}
void	DeleteStaticFile::closed() {}
