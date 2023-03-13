#include <Ressource.hpp>
#include <sstream>
/**************************************************************************/
/******************************** GET FILE ********************************/
/**************************************************************************/

GetStaticFile::GetStaticFile(Connexion *conn, std::string file_path) : Ressource(conn)
{
	fd_read = open(file_path.c_str(), O_RDONLY | O_NONBLOCK);

	Logger::info << conn->client_ip_addr << " - get static file " << file_path << std::endl;

	if (fd_read == -1)
		throw IOExcept("Error opening the file" + file_path, 404);

	if (set_nonblocking(fd_read))
		throw IOExcept("Error setting the file to non-blocking" + file_path, 500);

	if (poll_util(POLL_CTL_ADD, fd_read, this, POLLIN))
		throw IOExcept("Error adding the file to the poll", 500);

	struct stat st;

	if (fstat(fd_read, &st) == -1)
		throw IOExcept("Error getting the file size for " + file_path, 404);

	std::string header = http_header_formatter(200, st.st_size, get_mime(file_path));

	conn->pushResponse(header);
}

GetStaticFile::~GetStaticFile()
{
	poll_util(POLL_CTL_DEL, fd_read, this, POLLIN);
	if (close(fd_read) == -1)
		throw IOExcept("Error closing the file", 500);
}

/**************************************************************************/
/******************************** POST FILE *******************************/
/**************************************************************************/

PostStaticFile::PostStaticFile(Connexion *conn, std::string file_path) : Ressource(conn)
{
	bool	file_empty = !conn->getRequest().content_length;
	Logger::info << conn->client_ip_addr << " - post static file " << file_path << std::endl;

	fd_write = open(file_path.c_str(), O_WRONLY | O_EXCL | O_CREAT | O_NONBLOCK, CH_PERM);

	if (fd_write == -1)
		throw IOExcept("Error opening the file" + file_path, 403);
	if (set_nonblocking(fd_write))
		throw IOExcept("Error setting the file to non-blocking" + file_path, 500);

	if (!file_empty && poll_util(POLL_CTL_ADD, fd_write, this, POLLOUT))
		throw IOExcept("Error adding the file to the poll", 500);

	std::string header = http_header_formatter(200, 0, get_mime(file_path));

	conn->pushResponse(header);

	if (file_empty)
		conn->setRespEnd();

	Logger::debug << "PostStaticFile::PostStaticFile() OK" << std::endl;
}

PostStaticFile::~PostStaticFile()
{
	poll_util(POLL_CTL_DEL, fd_write, this, POLLOUT);
	if (close(fd_write) == -1)
		throw IOExcept("Error closing the file", 500);
}

/**************************************************************************/
/******************************* DELETE FILE ******************************/
/**************************************************************************/

DeleteStaticFile::DeleteStaticFile(Connexion *conn, std::string file_path) : Ressource(conn)
{
	Logger::info << conn->client_ip_addr << " - delete static file " << file_path << std::endl;

	int rm = remove(file_path.c_str());

	if (rm != 0)
		throw IOExcept("Error deleting the file" + file_path, 204);

	std::string body = "<html>\n";
	body += "<head>\n";
	body += "<title>File deleted</title>\n";
	body += "</head>\n";
	body += "<body>\n";
	body += "<h1>File deleted</h1>\n";
	body += "</body>\n";
	body += "</html>\n";
	conn->pushResponse(http_header_formatter(200, body.length(), "text/html"));
	conn->pushResponse(body);
	conn->setRespEnd();
}

DeleteStaticFile::~DeleteStaticFile() {}
