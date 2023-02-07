#include <Ressource.hpp>

GetStaticFile::GetStaticFile(Connexion *conn, std::string file_path) :	conn(conn),
																		fd_write(-1),
																		fd_read(open(file_path.c_str(), O_RDONLY)),
																		is_EOF(false)
{
}

GetStaticFile::~GetStaticFile() {}
void	GetStaticFile::read() {}
void	GetStaticFile::write() {}
void	GetStaticFile::closed() {}

PostStaticFile::PostStaticFile(Connexion *conn, std::string file_path) :	conn(conn),
																			fd_read(-1),
																			fd_write(open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, CH_PERM))
																			is_EOF(false)
{
}

PostStaticFile::~PostStaticFile() {}
void	PostStaticFile::read() {}
void	PostStaticFile::write() {}
void	PostStaticFile::closed() {}

DeleteStaticFile::DeleteStaticFile(Connexion *conn) :	conn(conn),
														fd_read(-1),
														fd_write(-1)
														is_EOF(false)
{
}

DeleteStaticFile::~DeleteStaticFile(Connexion *conn, std::string file_path) {}
void	DeleteStaticFile::read() {}
void	DeleteStaticFile::write() {}
void	DeleteStaticFile::closed() {}
