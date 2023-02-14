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
		throw std::runtime_error("GetStaticFile::GetStaticFile() failed");
	}
	set_nonblocking(fd_read);

	struct stat st;

	if (fstat(fd_read, &st) == -1)
	{
		conn->setError("Error getting the file size for " + file_path, 404);
		throw std::runtime_error("GetStaticFile::GetStaticFile() failed");
	}

	std::string header = "HTTP/1.1 200 OK\r\n";
	header += "Content-Type: " + get_mime(file_path) + "\r\n";
	header += "Content-Length: " + std::to_string(st.st_size) + "\r\n";
	header += "Connection: keep-alive\r\n\r\n";

	conn->append_response(header.c_str(), header.size());
}

GetStaticFile::~GetStaticFile()
{
	epoll_util(EPOLL_CTL_DEL, fd_read, this, EPOLLIN);
	if (close(fd_read) == -1)
	{
		conn->setError("Error closing the file", 500);
		throw std::runtime_error("GetStaticFile::~GetStaticFile() failed");
	}
}

IOEvent	GetStaticFile::read()
{
	memset(buffer, 0, BUFFER_SIZE); // instead of memset, add /0 at the end of the read/write
	size_t ret = ::read(fd_read, buffer, BUFFER_SIZE);

	if (ret == -1)
		return conn->setError("Error reading the file", 500);
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
	//do we correctly check if max_body_size is exceeded?

	fd_write = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK, CH_PERM);

	if (fd_write == -1)
	{
		conn->setError("Error opening the file" + file_path, 404);
		throw std::runtime_error("GetStaticFile::GetStaticFile() failed");
	}
	set_nonblocking(fd_write);

	std::string header = "HTTP/1.1 200 OK\r\n";
	header += "Content-Type: " + get_mime(file_path) + "\r\n";
	header += "Connection: keep-alive\r\n\r\n";

	conn->append_response(header.c_str(), header.size());
}

PostStaticFile::~PostStaticFile()
{
	epoll_util(EPOLL_CTL_DEL, fd_write, this, EPOLLIN);
	if (close(fd_write) == -1)
	{
		conn->setError("Error closing the file", 500);
		throw std::runtime_error("GetStaticFile::~GetStaticFile() failed");
	}
}

IOEvent	PostStaticFile::write()
{
	// need to check if the size is exceeding max_body_size?
	// more likely, don't check it but accept to receive a set number of bytes MAX
	// if no content length -> download til EOF OR max
	// if content length -> download til content length
	// conn->setError("File size is too big", 413);

	// Check if file exists

	// Parsing the body :
	// Need to handle decoding the body considering it's mime type in Content-type
	// if application/x-www-form-urlencoded extract data in key-value pairs :
		// eg name=John+Doe&email=john%40example.com&age=30
		// name: John Doe
		// email: john@example.com
		// age: 30
	// if multipart/form-data (chunked request) :

	size_t ret = ::write(fd_write, conn->getRequest().body.c_str(),
		conn->getRequest().body.size());

	if (ret == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return IOEvent();
		else
			return conn->setError("Error writing the file", 500);
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
