#include <Ressource.hpp>

/**************************************************************************/
/******************************** GET FILE ********************************/
/**************************************************************************/

GetStaticFile::GetStaticFile(Connexion *conn, std::string file_path) :	conn(conn),
																		fd_write(-1),
																		fd_read(open(file_path.c_str(), O_RDONLY)),
																		is_EOF(false)
{}

GetStaticFile::~GetStaticFile()
{
	epoll_util(EPOLL_CTL_DEL, fd_read, EPOLLIN);
	close(fd_read);
}

void	GetStaticFile::read()
{
	char	buffer[BUFFER_SIZE];
	size_t	ret;

	ret = recv(fd_read, buffer, BUFFER_SIZE);
	response.append(buffer, ret);
	if (ret == 0)
		is_EOF = true; // need more error handling
}

void	GetStaticFile::closed()
{
	throw std::runtime_error("GetStaticFile::closed() called");
}

t_fd	GetStaticFile::fdDelete() { return conn; }

/**************************************************************************/
/******************************** POST FILE *******************************/
/**************************************************************************/

PostStaticFile::PostStaticFile(Connexion *conn, std::string file_path) :	conn(conn),
																			fd_read(-1),
																			fd_write(open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, CH_PERM))
																			is_EOF(false)
{}

PostStaticFile::~PostStaticFile()
{
	epoll_util(EPOLL_CTL_DEL, fd_write, EPOLLIN);
	close(fd_write);
}

void	PostStaticFile::write()
{
	if (send(fd_write, conn->t_http_message.body[0],
		conn->t_http_message.body.size(), MSG_DONTWAIT) == -1) //con->t_http_message.body.data()?
		throw std::runtime_error("PostStaticFile::write() failed");
}
void	PostStaticFile::closed()
{
	throw std::runtime_error("PostStaticFile::closed() called");
}
t_fd	PostStaticFile::fdDelete() { return conn; }

/**************************************************************************/
/******************************* DELETE FILE ******************************/
/**************************************************************************/

DeleteStaticFile::DeleteStaticFile(Connexion *conn, std::string file_path) :	conn(conn),
																				fd_read(-1),
																				fd_write(-1)
																				is_EOF(false)
{}

DeleteStaticFile::~DeleteStaticFile() {}
void	DeleteStaticFile::closed()
{
	throw std::runtime_error("DeleteStaticFile::closed() called");
}
t_fd	DeleteStaticFile::fdDelete() { return conn; }
