#include <Ressource.hpp>

/**************************************************************************/
/******************************** GET FILE ********************************/
/**************************************************************************/

GetStaticFile::GetStaticFile(Connexion *conn, std::string file_path) :	Ressource(conn)
{
	fd_read = open(file_path.c_str(), O_RDONLY | O_NONBLOCK);

	if (fd_read == -1)
	{
		conn->setError("Error opening the file" + file_path, 404);
		throw std::runtime_error("GetStaticFile::GetStaticFile() Open failed");
	}
	set_nonblocking(fd_read);

	struct stat st;

	if (fstat(fd_read, &st) == -1)
	{
		conn->setError("Error getting the file size for " + file_path, 404);
		throw std::runtime_error("GetStaticFile::GetStaticFile() Fstat failed");
	}

	std::string header = "HTTP/1.1 200 OK\r\n";
	header += "Content-Type: " + get_mime(file_path) + CRLF;
	header += "Content-Length: " + std::to_string(st.st_size) + CRLF;
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

IOEvent	GetStaticFile::read()
{
	size_t ret = ::read(fd_read, buffer, BUFFER_SIZE);

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

IOEvent	GetStaticFile::closed()
{
	return IOEvent(FAIL, this, "GetStaticFile::closed() called");
}

/**************************************************************************/
/******************************** POST FILE *******************************/
/**************************************************************************/

PostStaticFile::PostStaticFile(Connexion *conn, std::string file_path) :	Ressource(conn)
{
	std::string new_path = file_path;

	fd_write = open(new_path.c_str(), O_WRONLY | O_EXCL | O_CREAT | O_NONBLOCK, CH_PERM);

	if (fd_write == -1)
	{
		conn->setError("Error opening the file" + file_path, 404);
		throw std::runtime_error("PostStaticFile::PostStaticFile() Open failed");
	}
	set_nonblocking(fd_write);

	std::string header = "HTTP/1.1 200 OK\r\n";
	header += "Content-Type: " + get_mime(new_path) + CRLF;
	header += "Connection: keep-alive\r\n\r\n";

	conn->append_response(header.c_str(), header.size());
}

PostStaticFile::~PostStaticFile()
{
	epoll_util(EPOLL_CTL_DEL, fd_write, this, EPOLLIN);
	if (close(fd_write) == -1)
	{
		conn->setError("Error closing the file", 500);
		throw std::runtime_error("PostStaticFile::~PostStaticFile() Close failed");
	}
}

IOEvent	PostStaticFile::write()
{
	size_t ret = ::write(fd_write, conn->getRequest().body.c_str(),
		conn->getRequest().body.size());

	if (ret <= 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return IOEvent();
		else
		{
			close(fd_write);
			return conn->setError("Error writing the file", 500);
		}
	}
}

IOEvent	PostStaticFile::closed()
{
	return IOEvent(FAIL, this, "PostStaticFile::closed() called");
}

/**************************************************************************/
/******************************* DELETE FILE ******************************/
/**************************************************************************/

DeleteStaticFile::DeleteStaticFile(Connexion *conn, std::string file_path) :	Ressource(conn)
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
