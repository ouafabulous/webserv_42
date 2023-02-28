#include <Ressource.hpp>
#include <sstream>
/**************************************************************************/
/******************************** GET FILE ********************************/
/**************************************************************************/

GetStaticFile::GetStaticFile(Connexion *conn, std::string file_path) : Ressource(conn)
{
	fd_read = open(file_path.c_str(), O_RDONLY | O_NONBLOCK);

	if (fd_read == -1)
	{
		conn->setError("Error opening the file" + file_path, 404);
		throw std::runtime_error("GetStaticFile::GetStaticFile() Open failed");
	}
	set_nonblocking(fd_read);
	if (!epoll_util(EPOLL_CTL_ADD, fd_read, this, EPOLLIN))
	{
		conn->setError("Error adding the file to the epoll", 500);
		throw std::runtime_error("GetStaticFile::GetStaticFile() epoll_util failed");
	}

	struct stat st;

	if (fstat(fd_read, &st) == -1)
	{
		conn->setError("Error getting the file size for " + file_path, 404);
		throw std::runtime_error("GetStaticFile::GetStaticFile() Fstat failed");
	}

	std::string header = "HTTP/1.1 200 OK\r\n";
	header += "Content-Type: " + get_mime(file_path) + CRLF;
	std::ostringstream s;
	s << st.st_size;
	std::string	s_size(s.str());
	header += "Content-Length: " + s_size + CRLF;
	header += "Connection: closed\r\n\r\n"; // or keep-alive ?

	conn->append_response(header.c_str(), header.size());
}

GetStaticFile::~GetStaticFile()
{
	epoll_util(EPOLL_CTL_DEL, fd_read, this, EPOLLIN);
	if (close(fd_read) == -1)
	{
		conn->setError("Error closing the file", 500);
		throw std::runtime_error("GetStaticFile::~GetStaticFile() Close failed");
	}
}

IOEvent GetStaticFile::read()
{
	int	ret = ::read(fd_read, buffer, BUFFER_SIZE);

	if (ret == -1)
	{
		close(fd_read);
		return conn->setError("Error reading the file", 500);
	}
	if (!ret)
		return closed();
	if (ret == 0)
		is_EOF = true;
	conn->append_response(buffer, ret);
	return IOEvent();
}

IOEvent GetStaticFile::closed()
{
	return conn->setError("Error reading the file", 500);
}

/**************************************************************************/
/******************************** POST FILE *******************************/
/**************************************************************************/

PostStaticFile::PostStaticFile(Connexion *conn, std::string file_path) : Ressource(conn)
{
	std::string new_path = file_path;

	fd_write = open(new_path.c_str(), O_WRONLY | O_EXCL | O_CREAT | O_NONBLOCK, CH_PERM);

	if (fd_write == -1)
	{
		conn->setError("Error opening the file" + file_path, 404);
		throw std::runtime_error("PostStaticFile::PostStaticFile() Open failed");
	}
	set_nonblocking(fd_write);
	if (!epoll_util(EPOLL_CTL_ADD, fd_write, this, EPOLLOUT))
	{
		conn->setError("Error adding the file to the epoll", 500);
		throw std::runtime_error("PostStaticFile::PostStaticFile() epoll_util failed");
	}

	std::string header = "HTTP/1.1 200 OK\r\n";
	header += "Content-Type: " + get_mime(new_path) + CRLF;
	header += "Connection: keep-alive\r\n\r\n";

	conn->append_response(header.c_str(), header.size());
}

PostStaticFile::~PostStaticFile()
{
	epoll_util(EPOLL_CTL_DEL, fd_write, this, EPOLLOUT);
	if (close(fd_write) == -1)
	{
		conn->setError("Error closing the file", 500);
		throw std::runtime_error("PostStaticFile::~PostStaticFile() Close failed");
	}
}

IOEvent PostStaticFile::write()
{
	if (conn->getRequest().body.empty())
		return IOEvent();

	int	ret = ::write(fd_write, conn->getRequest().body.c_str(),
							conn->getRequest().body.size());

	// if ret > 0

	if (!ret)
	{
		is_EOF = true;
		return IOEvent();
	}
	if (ret < 0)
		return conn->setError("Error writing the file", 500);
	bytes_read += ret;
	return (IOEvent());
}

IOEvent PostStaticFile::closed()
{
	return conn->setError("Error writing the file", 500);
}

/**************************************************************************/
/******************************* DELETE FILE ******************************/
/**************************************************************************/

DeleteStaticFile::DeleteStaticFile(Connexion *conn, std::string file_path) : Ressource(conn)
{
	int rm = remove(file_path.c_str());
	if (rm != 0)
		throw std::runtime_error("DeleteStaticFile::DeleteStaticFile() failed");
}

DeleteStaticFile::~DeleteStaticFile() {}
IOEvent DeleteStaticFile::closed()
{
	return conn->setError("Error deleting the file", 500);
}
