#include <Utils.hpp>
#include <Server.hpp>

#define SUCCESS 0
#define ERROR 1

bool	set_nonblocking(int fd)
{
	if (fd <0)
		return ERROR;
	int	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return ERROR;
	return SUCCESS;
}


bool	epoll_util(int action, t_fd fd, IO* io_ptr, int flags) {
	epoll_event	event;
	event.data.ptr = io_ptr;
	event.events = flags;
	if (epoll_ctl(Server::epollfd, action, fd, &event) == -1)
		return ERROR;
	return SUCCESS;
}

std::string http_header_formatter(uint status_code, size_t content_length) {
	static std::map<uint, std::string>	status_text;
	std::stringstream 					result;

	if (status_text.empty()) {
		status_text[100] = "Continue";
		status_text[101] = "Switching Protocols";
		status_text[200] = "OK";
		status_text[201] = "Created";
		status_text[202] = "Accepted";
		status_text[203] = "Non-Authoritative Information";
		status_text[204] = "No Content";
		status_text[205] = "Reset Content";
		status_text[206] = "Partial Content";
		status_text[300] = "Multiple Choices";
		status_text[301] = "Moved Permanently";
		status_text[302] = "Found";
		status_text[303] = "See Other";
		status_text[304] = "Not Modified";
		status_text[305] = "Use Proxy";
		status_text[307] = "Temporary Redirect";
		status_text[400] = "Bad Request";
		status_text[401] = "Unauthorized";
		status_text[402] = "Payment Required";
		status_text[403] = "Forbidden";
		status_text[404] = "Not Found";
		status_text[405] = "Method Not Allowed";
		status_text[406] = "Not Acceptable";
		status_text[407] = "Proxy Authentication Required";
		status_text[408] = "Request Timeout";
		status_text[409] = "Conflict";
		status_text[410] = "Gone";
		status_text[411] = "Length Required";
		status_text[412] = "Precondition Failed";
		status_text[413] = "Request Entity Too Large";
		status_text[414] = "Request-URI Too Long";
		status_text[415] = "Unsupported Media Type";
		status_text[416] = "Requested Range Not Satisfiable";
		status_text[417] = "Expectation Failed";
		status_text[500] = "Internal Server Error";
		status_text[501] = "Not Implemented";
		status_text[502] = "Bad Gateway";
		status_text[503] = "Service Unavailable";
		status_text[504] = "Gateway Timeout";
		status_text[505] = "HTTP Version Not Supported";
	}

	result << "HTTP/1.1 " << status_code << " " << status_text[status_code] << CRLF;
	result << "Content-Length: " << content_length << CRLF;
	result << CRLF;

	return result.str();
}

// STREAM OVERLOAD

std::ostream&	operator<<(std::ostream &out, const t_network_address &netAddr)
{
	const static std::string	text[5] = {"TOK_WORD", "TOK_SP\t", "TOK_RL\t", "TOK_SC\t", "TOK_BR\t"};

	if (netAddr.first) {
		unsigned char *p = (unsigned char *)&netAddr.first;
		out << (int)p[0] << "." << (int)p[1] << "." << (int)p[2] << "." << (int)p[3] << ":";
	}
	out << ntohs(netAddr.second);
    return out;
}

int stringToInt(std::string str){
    char *end;
    int number = strtol(str.c_str(), &end, 10);
    if (end == str.c_str() + strlen(str.c_str()) && number >= 0) {
        return(number);
    } else {
        throw std::runtime_error("Port is not a positive number !\n");
    }
    return 0;
}
