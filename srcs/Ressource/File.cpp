#include <Ressource.hpp>
#include <sstream>
/**************************************************************************/
/******************************** GET FILE ********************************/
/**************************************************************************/

GetStaticFile::GetStaticFile(Connexion *conn, std::string file_path) : Ressource(conn)
{
	fd_read = open(file_path.c_str(), O_RDONLY | O_NONBLOCK);

	Logger::info << "Get static file " << file_path << std::endl;

	if (fd_read == -1)
	{
		conn->setError("Error opening the file" + file_path, 404);
		throw std::runtime_error("GetStaticFile::GetStaticFile() Open failed");
	}

	if (set_nonblocking(fd_read))
	{
		conn->setError("Error setting the file to non-blocking", 500);
		throw std::runtime_error("GetStaticFile::GetStaticFile() set_nonblocking failed");
	}
	if (poll_util(POLL_CTL_ADD, fd_read, this, POLLIN))
	{
		conn->setError("Error adding the file to the poll", 500);
		throw std::runtime_error("GetStaticFile::GetStaticFile() poll_util failed");
	}

	struct stat st;

	if (fstat(fd_read, &st) == -1)
	{
		conn->setError("Error getting the file size for " + file_path, 404);
		throw std::runtime_error("GetStaticFile::GetStaticFile() Fstat failed");
	}

	std::string header = http_header_formatter(200, st.st_size, get_mime(file_path));
	//header += "Connection: closed\r\n\r\n";

	conn->pushResponse(header);
}

GetStaticFile::~GetStaticFile()
{
	poll_util(POLL_CTL_DEL, fd_read, this, POLLIN);
	if (close(fd_read) == -1)
	{
		conn->setError("Error closing the file", 500);
		throw std::runtime_error("GetStaticFile::~GetStaticFile() Close failed");
	}
}

/**************************************************************************/
/******************************** POST FILE *******************************/
/**************************************************************************/

PostStaticFile::PostStaticFile(Connexion *conn, std::string file_path) : Ressource(conn)
{
	Logger::info << "PostStaticFile::PostStaticFile()" << file_path << std::endl;

	fd_write = open(file_path.c_str(), O_WRONLY | O_EXCL | O_CREAT | O_NONBLOCK, CH_PERM);

	if (fd_write == -1)
	{
		conn->setError("Error opening the file" + file_path, 404);
		throw std::runtime_error("PostStaticFile::PostStaticFile() Open failed");
	}
	if (set_nonblocking(fd_write))
	{
		conn->setError("Error setting the file to non-blocking", 500);
		throw std::runtime_error("PostStaticFile::PostStaticFile() set_nonblocking failed");
	}
	if (poll_util(POLL_CTL_ADD, fd_write, this, POLLOUT))
	{
		conn->setError("Error adding the file to the poll", 500);
		throw std::runtime_error("PostStaticFile::PostStaticFile() poll_util failed");
	}

	std::string header = http_header_formatter(200, 0, get_mime(file_path));
	//header += "Connection: closed\r\n\r\n";

	conn->pushResponse(header);


	Logger::debug << "PostStaticFile::PostStaticFile() OK" << std::endl;
}

PostStaticFile::~PostStaticFile()
{
	poll_util(POLL_CTL_DEL, fd_write, this, POLLOUT);
	if (close(fd_write) == -1)
	{
		conn->setError("Error closing the file", 500);
		throw std::runtime_error("PostStaticFile::~PostStaticFile() Close failed");
	}
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
