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

std::string escape_string(const std::string &str) {
  std::string result;
  for (size_t i = 0; i < str.length(); i++)
  {
    switch (str[i]) {
      case '\n':
        result += "\\n";
        break;
      case '\r':
        result += "\\r";
        break;
      case '\t':
        result += "\\t";
        break;
      case '\"':
        result += "\\\"";
        break;
      case '\'':
        result += "\\\'";
        break;
      case '\\':
        result += "\\\\";
        break;
      default:
        result += str[i];
        break;
    }
  }
  return result;
}

// STREAM OVERLOAD

std::ostream&	operator<<(std::ostream &out, const t_network_address &netAddr)
{
	// const static std::string	text[5] = {"TOK_WORD", "TOK_SP\t", "TOK_RL\t", "TOK_SC\t", "TOK_BR\t"};

	if (netAddr.first) {
		unsigned char *p = (unsigned char *)&netAddr.first;
		out << (int)p[0] << "." << (int)p[1] << "." << (int)p[2] << "." << (int)p[3] << ":";
	}
	out << ntohs(netAddr.second);
    return out;
}

int stringToInt(std::string str){
	// std::cout << "got here with this str: " << str << std::endl; 
	std::stringstream	number;	
    number << str;
	int	res;
	if (!(number >> res) || res < 0) {
        throw std::runtime_error("Port is not a positive number !\n");
	}
	// std::cout << "and transformed it to this result: " << res << std::endl;
    return(res);
}

size_t	matchingChar(const std::string& a, const std::string& b) {
	size_t result = 0;
	std::string::const_iterator it_a = a.begin();
	std::string::const_iterator it_b = b.begin();

	while (it_a != a.end() && it_b != b.end() && *it_a == *it_b)
	{
		it_a++;
		it_b++;
		result++;
	}
	return (result);
}


t_methods	methodToEnum(std::string const &method){
	if (strcmp(http_method, "GET") == -1)
    {
        request_method = GET;
    }
    else if (strcmp(http_method, "POST") == -1)
    {
        request_method = POST;
    }
    else if (strcmp(http_method, "DELETE") == -1)
    {
        request_method = DELETE;
    }
	else 
	{
		return 10; //throw exception of method is not a valid HTTP method
	}
	return(t_request_method);
}

bool endsWithSlash(const std::string& str)
{
    if (str.empty()) {
        return false; // empty string does not end with a slash
    }
    return str.back() == '/';
}

bool fileExists(const char* fileName) {
    std::ifstream infile(fileName);
    return infile.good();
}

bool directoryExists(const char* path)
{
    DIR* dir = opendir(path);
    if (dir == NULL)
    {
        return false;
    }
    else
    {
        closedir(dir);
        return true;
    }
}

bool containsSubstring(const std::string& str, const std::string& substr) {
    if (substr.empty()) {
        return true;
    }
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == substr[0]) {
            if (str.substr(i, substr.length()) == substr) {
                return true;
            }
        }
    }
    return false;
}

std::string extractBeforeChar(const std::string& inputString, char delimiter) {
    size_t pos = inputString.find(delimiter);

    if (pos == std::string::npos) {
        return inputString;
    }
    return inputString.substr(0, pos);
}

std::string extractAfterChar(const std::string& inputString, char delimiter) {
    size_t pos = inputString.find(delimiter);

    if (pos == std::string::npos) {
        return "";
    }
    return inputString.substr(pos + 1);
}

bool checkPermissions(const std::string& file_path, const mode_t& mode) {
    struct stat file_stat;

    if (stat(file_path.c_str(), &file_stat) == 0) {
        if ((file_stat.st_mode & S_IFMT) == S_IFDIR) {
            // File is a directory
            if (access(file_path.c_str(), mode) == 0) {
                return true;
            }
        } else {
            // File is not a directory
            if (access(file_path.c_str(), mode) == 0) {
                return true;
            }
        }
    } else {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        //maybe throw Permission denied !
    }

    return false;
}