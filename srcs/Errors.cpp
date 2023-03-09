#include <Type.hpp>
#include <Utils.hpp>
#include <iostream>
#include <fstream>
#include <Ressource.hpp>


/**************************************************************************/
/******************************** GET ERROR *******************************/
/**************************************************************************/

GetError::GetError(Connexion *conn, uint status_code, std::string file_path) : Ressource(conn), is_custom(false)
{
	std::string response;

	if (!file_path.empty() && access(file_path.c_str(), R_OK) == 0)
		is_custom = true;

	Logger::info << conn->client_ip_addr << " - getError " << status_code << (is_custom ? " (" + file_path + ")": "") << std::endl;

	if (is_custom) {
		fd_read = open(file_path.c_str(), O_RDONLY | O_NONBLOCK);

		if (fd_read == -1) {
			conn->setError("Error opening the file" + file_path, 404);
			throw std::runtime_error("GetError::GetError() Open failed");
		}

		if (set_nonblocking(fd_read)) {
			conn->setError("Error setting the file to non-blocking", 500);
			throw std::runtime_error("GetError::GetError() set_nonblocking failed");
		}
		if (poll_util(POLL_CTL_ADD, fd_read, this, POLLIN)) {
			conn->setError("Error adding the file to the poll", 500);
			throw std::runtime_error("GetError::GetError() poll_util failed");
		}

		struct stat st;

		if (fstat(fd_read, &st) == -1) {
			conn->setError("Error getting the file size for " + file_path, 404);
			throw std::runtime_error("GetError::GetError() Fstat failed");
		}
		response = http_header_formatter(status_code, st.st_size, get_mime(file_path));
		conn->pushResponse(response);
	}
	else {
		response = http_header_formatter(status_code, getDefaultError(status_code).size(), "text/html") + getDefaultError(status_code);
		conn->pushResponse(response);
	}


}

GetError::~GetError()
{
	if (is_custom) {
		poll_util(POLL_CTL_DEL, fd_read, this, POLLIN);
		if (close(fd_read) == -1)
		{
			conn->setError("Error closing the file", 500);
			throw std::runtime_error("GetError::~GetError() Close failed");
		}
	}
}

const std::string&	GetError::getDefaultError(uint status_code) {
	t_errors_map::const_iterator	error_it;
	static t_errors_map			default_errors;

	if (default_errors.empty()) {
		default_errors[301] =
			"<html>" CRLF
			"<head><title>301 Moved Permanently</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>301 Moved Permanently</h1></center>" CRLF;

		default_errors[302] =
			"<html>" CRLF
			"<head><title>302 Found</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>302 Found</h1></center>" CRLF;

		default_errors[303] =
			"<html>" CRLF
			"<head><title>303 See Other</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>303 See Other</h1></center>" CRLF;

		default_errors[307] =
			"<html>" CRLF
			"<head><title>307 Temporary Redirect</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>307 Temporary Redirect</h1></center>" CRLF;

		default_errors[308] =
			"<html>" CRLF
			"<head><title>308 Permanent Redirect</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>308 Permanent Redirect</h1></center>" CRLF;

		default_errors[400] =
			"<html>" CRLF
			"<head><title>400 Bad Request</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>400 Bad Request</h1></center>" CRLF;

		default_errors[401] =
			"<html>" CRLF
			"<head><title>401 Authorization Required</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>401 Authorization Required</h1></center>" CRLF;

		default_errors[402] =
			"<html>" CRLF
			"<head><title>402 Payment Required</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>402 Payment Required</h1></center>" CRLF;

		default_errors[403] =
			"<html>" CRLF
			"<head><title>403 Forbidden</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>403 Forbidden</h1></center>" CRLF;

		default_errors[404] =
			"<html>" CRLF
			"<head><title>404 Not Found</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>404 Not Found</h1></center>" CRLF;

		default_errors[405] =
			"<html>" CRLF
			"<head><title>405 Not Allowed</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>405 Not Allowed</h1></center>" CRLF;

		default_errors[406] =
			"<html>" CRLF
			"<head><title>406 Not Acceptable</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>406 Not Acceptable</h1></center>" CRLF;

		default_errors[408] =
			"<html>" CRLF
			"<head><title>408 Request Time-out</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>408 Request Time-out</h1></center>" CRLF;

		default_errors[409] =
			"<html>" CRLF
			"<head><title>409 Conflict</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>409 Conflict</h1></center>" CRLF;

		default_errors[410] =
			"<html>" CRLF
			"<head><title>410 Gone</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>410 Gone</h1></center>" CRLF;

		default_errors[411] =
			"<html>" CRLF
			"<head><title>411 Length Required</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>411 Length Required</h1></center>" CRLF;

		default_errors[412] =
			"<html>" CRLF
			"<head><title>412 Precondition Failed</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>412 Precondition Failed</h1></center>" CRLF;

		default_errors[413] =
			"<html>" CRLF
			"<head><title>413 Request Entity Too Large</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>413 Request Entity Too Large</h1></center>" CRLF;

		default_errors[414] =
			"<html>" CRLF
			"<head><title>414 Request-URI Too Large</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>414 Request-URI Too Large</h1></center>" CRLF;

		default_errors[415] =
			"<html>" CRLF
			"<head><title>415 Unsupported Media Type</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>415 Unsupported Media Type</h1></center>" CRLF;

		default_errors[416] =
			"<html>" CRLF
			"<head><title>416 Requested Range Not Satisfiable</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>416 Requested Range Not Satisfiable</h1></center>" CRLF;

		default_errors[421] =
			"<html>" CRLF
			"<head><title>421 Misdirected Request</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>421 Misdirected Request</h1></center>" CRLF;

		default_errors[429] =
			"<html>" CRLF
			"<head><title>429 Too Many Requests</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>429 Too Many Requests</h1></center>" CRLF;

		default_errors[494] =
			"<html>" CRLF
			"<head><title>400 Request Header Or Cookie Too Large</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>400 Bad Request</h1></center>" CRLF
			"<center>Request Header Or Cookie Too Large</center>" CRLF;

		default_errors[495] =
			"<html>" CRLF
			"<head><title>400 The SSL certificate error</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>400 Bad Request</h1></center>" CRLF
			"<center>The SSL certificate error</center>" CRLF;

		default_errors[496] =
			"<html>" CRLF
			"<head><title>400 No required SSL certificate was sent</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>400 Bad Request</h1></center>" CRLF
			"<center>No required SSL certificate was sent</center>" CRLF;

		default_errors[497] =
			"<html>" CRLF
			"<head><title>400 The plain HTTP request was sent to HTTPS port</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>400 Bad Request</h1></center>" CRLF
			"<center>The plain HTTP request was sent to HTTPS port</center>" CRLF;

		default_errors[500] =
			"<html>" CRLF
			"<head><title>500 Internal Server Error</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>500 Internal Server Error</h1></center>" CRLF;

		default_errors[501] =
			"<html>" CRLF
			"<head><title>501 Not Implemented</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>501 Not Implemented</h1></center>" CRLF;

		default_errors[502] =
			"<html>" CRLF
			"<head><title>502 Bad Gateway</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>502 Bad Gateway</h1></center>" CRLF;

		default_errors[503] =
			"<html>" CRLF
			"<head><title>503 Service Temporarily Unavailable</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>503 Service Temporarily Unavailable</h1></center>" CRLF;

		default_errors[504] =
			"<html>" CRLF
			"<head><title>504 Gateway Time-out</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>504 Gateway Time-out</h1></center>" CRLF;

		default_errors[505] =
			"<html>" CRLF
			"<head><title>505 HTTP Version Not Supported</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>505 HTTP Version Not Supported</h1></center>" CRLF;

		default_errors[507] =
			"<html>" CRLF
			"<head><title>507 Insufficient Storage</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>507 Insufficient Storage</h1></center>" CRLF;
	}

	error_it = default_errors.find(status_code);
	if (error_it != default_errors.end()) {
		return error_it->second;
	}
	return default_errors[500];
}

std::ostream&	operator<<(std::ostream &out, const t_errors_map &errors)
{
	for (t_errors_map::const_iterator it = errors.begin(); it != errors.end(); it++)
		out << "(" << it->first << ": " << it->second << ") ";
    return out;
}
