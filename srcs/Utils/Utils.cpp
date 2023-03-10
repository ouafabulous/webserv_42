#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
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


bool	poll_util(t_poll_action action, t_fd fd, IO* io_ptr, int flags) {
	if (action == POLL_CTL_ADD) {
		t_pollfd	new_pollfd;
		new_pollfd.events = flags;
		new_pollfd.fd = fd;
		new_pollfd.revents = 0;

		Server::pollfds.push_back(new_pollfd);
		Server::socks[fd] = io_ptr;
		return OK;
	}

	Server::t_poll_it		poll_it = Server::pollfds.begin();
	for (poll_it = Server::pollfds.begin(); poll_it != Server::pollfds.end(); poll_it++)
	{
		if (poll_it->fd == fd)
			break;
	}
	if (poll_it == Server::pollfds.end())
		return NOK;

	if (action == POLL_CTL_MOD)
		poll_it->events = flags;
	else if (action == POLL_CTL_DEL) {
		Server::pollfds.erase(poll_it);
		Server::socks[fd] = NULL;
		// Server::socks.erase(fd);
	}
	return SUCCESS;
}

std::string http_header_formatter(uint status_code, size_t content_length,std::string content_type, t_headers custom_headers) {
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
	result << "Content-Type: " << content_type << CRLF;
	result << "Content-Length: " << content_length << CRLF;
	for (t_headers::iterator it = custom_headers.begin(); it != custom_headers.end(); it++)
		result << it->first << ": " << it->second << CRLF;
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

size_t	matchingLocation(const std::string& path, const std::string& location) {
	size_t result = 0;
	std::string::const_iterator it_path = path.begin();
	std::string::const_iterator it_location = location.begin();

	while (it_path != path.end() && it_location != location.end() && *it_path == *it_location)
	{
		it_path++;
		it_location++;
		result++;
	}
	if (it_location != location.end())
		return (0);
	return (result);
}


t_methods	methodToEnum(std::string const &http_method){
    t_methods   request_method;

	if (!strcmp(http_method.c_str(), "GET"))
    {
        request_method = GET;
    }
    else if (!strcmp(http_method.c_str(), "POST"))
    {
        request_method = POST;
    }
    else if (!strcmp(http_method.c_str(), "DELETE"))
    {
        request_method = DELETE;
    }
	else
	{
		throw std::runtime_error("valid methods are GET, POST & DELETE!\n");
	}
	return(request_method);
}

// bool endsWithSlash(const std::string& str)
// {
//     if (str.empty()) {
//         return false; // empty string does not end with a slash
//     }
//     return str.back() == '/';
// }

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

bool checkPermissions(const std::string& file_path, const mode_t& mode)
{
	if (access(file_path.c_str(), mode) == 0)
		return true;
	Logger::warning << "Error: " << strerror(errno) << std::endl;
	return false;
}


const MimeMap &get_mime_map()
{
	static MimeMap mime_map;

	if (mime_map.empty())
	{
		mime_map[".html"] = "text/html";
		mime_map[".htm"] = "text/html";
		mime_map[".css"] = "text/css";
		mime_map[".js"] = "application/javascript";
		mime_map[".jpg"] = "image/jpeg";
		mime_map[".jpeg"] = "image/jpeg";
		mime_map[".png"] = "image/png";
		mime_map[".gif"] = "image/gif";
		mime_map[".svg"] = "image/svg+xml";
		mime_map[".txt"] = "text/plain";
		mime_map[".pdf"] = "application/pdf";
		mime_map[".zip"] = "application/zip";
		mime_map[".gz"] = "application/gzip";
		mime_map[".tar"] = "application/x-tar";
		mime_map[".mp3"] = "audio/mpeg";
		mime_map[".mp4"] = "video/mp4";
		mime_map[".mpeg"] = "video/mpeg";
		mime_map[".mpg"] = "video/mpeg";
		mime_map[".avi"] = "video/x-msvideo";
		mime_map[".wmv"] = "video/x-ms-wmv";
		mime_map[".mov"] = "video/quicktime";
		mime_map[".flv"] = "video/x-flv";
		mime_map[".swf"] = "application/x-shockwave-flash";
		mime_map[".webm"] = "video/webm";
		mime_map[".ogg"] = "video/ogg";
		mime_map[".ogv"] = "video/ogg";
		mime_map[".oga"] = "audio/ogg";
		mime_map[".ogx"] = "application/ogg";
		mime_map[".aac"] = "audio/aac";
		mime_map[".wav"] = "audio/wav";
		mime_map[".webp"] = "image/webp";
		mime_map[".ico"] = "image/x-icon";
		mime_map[".xml"] = "application/xml";
		mime_map[".json"] = "application/json";
	}
	return mime_map;
}

const	std::string			get_mime(const std::string &file_path)
{
	size_t pos = file_path.find_last_of('.');
	if (pos == std::string::npos)
		return "application/octet-stream; charset=utf-8";

	std::string extension = file_path.substr(pos);
	const MimeMap &mimeMap = get_mime_map();
	MimeMap::const_iterator it = mimeMap.find(extension);
	if (it == mimeMap.end())
		return "application/octet-stream; charset=utf-8";
	return it->second;
}

std::string	queueToStr(std::queue<std::string> q){
    std::string str;
    while (!q.empty()) {
        str += q.front();
        q.pop();
    }
	return (str);
}

void	createFolder(const std::string& folderName) {
    int status = mkdir(folderName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status != 0)
		throw std::runtime_error("Couldn't open " + folderName + "!\n");
}
