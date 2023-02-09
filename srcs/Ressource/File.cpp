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
	epoll_util(EPOLL_CTL_DEL, fd_read, this, EPOLLIN);
	close(fd_read);
}

IOEvent	GetStaticFile::read()
{
	char	buffer[BUFFER_SIZE];
	size_t	ret;

	ret = recv(fd_read, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	response.insert(response.end(), buffer, buffer + ret);
	if (ret == 0)
		is_EOF = true;
}

IOEvent	GetStaticFile::closed()
{
	return IOEvent(FAIL, this, "GetStaticFile::closed() called");
}

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
	epoll_util(EPOLL_CTL_DEL, fd_write, this, EPOLLIN);
	close(fd_write);
}

IOEvent	PostStaticFile::write()
{
	if (send(fd_write, conn->request.body[0],
		conn->request.body.size(), MSG_DONTWAIT) == -1) //con->t_http_message.body.data()?
		return IOEvent(FAIL, this, "PostStaticFile::write() failed");
}
IOEvent	PostStaticFile::closed()
{
	return IOEvent(FAIL, this, "PostStaticFile::closed() called");
}

/**************************************************************************/
/******************************* DELETE FILE ******************************/
/**************************************************************************/

DeleteStaticFile::DeleteStaticFile(Connexion *conn, std::string file_path) :	conn(conn),
																				fd_read(-1),
																				fd_write(-1)
																				is_EOF(false)
{
	int	rm = remove(file_path.c_str());
	if (rm != 0)
		throw std::runtime_error("DeleteStaticFile::DeleteStaticFile() failed");
}

DeleteStaticFile::~DeleteStaticFile() {}
IOEvent	DeleteStaticFile::closed()
{
	return IOEvent(FAIL, this, "DeleteStaticFile::closed() called");
}
