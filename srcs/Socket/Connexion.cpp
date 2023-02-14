#include <Socket.hpp>
#include <Ressource.hpp>

inline bool ends_with(std::string const &value, std::string const &ending)
{
	if (ending.size() > value.size())
		return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

// CONSTRUCTORS & DESTRUCTOR

Connexion::Connexion(const t_network_address netAddr,
					 const t_fd socket, const Router &router) : c_socket(socket),
																netAddr(netAddr),
																router(router),
																header_end(false),
																request_line_received(false),
																route(NULL),
																ressource(NULL),
																response_start(false)
{
}
Connexion::~Connexion()
{
	Logger::debug << c_socket << " was delete" << std::endl;
	if (epoll_util(EPOLL_CTL_DEL, c_socket, this, EPOLLIN))
		throw std::runtime_error("epoll_del failed");
	close(c_socket);
}

// IOEvent

IOEvent Connexion::read()
{
	ssize_t recv_size = recv(c_socket, buffer, BUFFER_SIZE, MSG_DONTWAIT);

	if (recv_size == -1)
		return IOEvent(FAIL, this, "Error happened while reading socket");
	// 	// request_header.append(buffer);
	if (!recv_size)
		return IOEvent(FAIL, this, "Client closed the connexion");
	buffer[recv_size] = 0;
	if (!header_end)
		return readHeader();
	// request.body.insert(request.body.end(), buffer, recv_size);
	// else
	// 	return readBody();
	// if (ends_with(request_header, "\n\r")) {
	// }
	return IOEvent();
}
IOEvent Connexion::write()
{
	if (response.empty())
		return IOEvent();
	Logger::debug << "write to conn" << std::endl;
	if (send(c_socket, response.c_str(), response.length(), MSG_DONTWAIT) == -1)
		return IOEvent(FAIL, this, "unable to write to the client socket");
	return IOEvent(SUCCESS, this, "successfuly send response");
}
IOEvent Connexion::closed() { return IOEvent(FAIL, this, "client closed the connexion"); }

// Underlying operations

IOEvent Connexion::setError(std::string log, uint http_error)
{
	std::string body;

	if (response_start)
		return IOEvent(FAIL, this, log);
	response.clear();
	Logger::warning << http_error << " " << log << std::endl;
	if (route)
		body = route->getError(http_error);
	else
		body = Errors::getDefaultError(http_error);
	response.append(http_header_formatter(http_error, body.length()));
	response.append(body);
	if (ressource)
	{
		delete ressource;
		ressource = NULL;
	}
	if (epoll_util(EPOLL_CTL_MOD, c_socket, this, EPOLLOUT))
		return IOEvent(FAIL, this, "unable to epoll_ctl_mod");
	return IOEvent();
}

IOEvent Connexion::readHeader()
{
	std::string::iterator line_start;
	std::string current_line;
	std::vector<std::string> lines;

	request_header.append(buffer);
	if (request_header.length() >= MAX_HEADER_SIZE)
		return setError("header exceeds max header size", 413);

	line_start = request_header.begin();
	for (std::string::iterator it = request_header.begin() + 1; it != request_header.end(); it++)
	{
		if (*(it - 1) == '\r' && *it == '\n')
		{
			if (line_start == it - 1)
			{ // Detect end of header (/r/n/r/n)
				header_end = true;
				break;
			}
			current_line.assign(line_start, it - 1);
			lines.push_back(current_line);
			line_start = it + 1;
		}
	}
	request_header.erase(request_header.begin(), line_start);
	if (lines.empty())
		return IOEvent();
	return parseHeader(lines);
}
IOEvent Connexion::parseHeader(std::vector<std::string> &lines)
{
	std::string&	curr_line = lines[0];
	size_t			pos = 0;
	if (!request_line_received) {
		std::vector<std::string>	request_line;
		while (pos != std::string::npos) {
			pos = curr_line.find(" ");
			request_line.push_back(curr_line.substr(0, pos));
			curr_line.erase(0, pos + 1);
		}
		if (request_line.size() != 3)
			return setError("request_line is invalid", 413);
		request.request_line.method = request_line[0];
		request.request_line.path = request_line[1];
		request.request_line.http_version = request_line[2];
		lines.erase(lines.begin());
		request_line_received = true;
	}
	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++) {
		if ((pos = (*it).find(":")) == std::string::npos)
			return setError("header fields not RFC conform", 413);
		request.header_fields[(*it).substr(0, pos)] = (*it).substr((*it).find_first_not_of(' ', pos + 1));
		Logger::debug << request.header_fields[(*it).substr(0, pos)] << std::endl;
	}
	return IOEvent();
}
// bool	Connexion::readBody() {}

t_http_message &Connexion::getRequest() { return request; }

void Connexion::append_response(std::string message, size_t n)
{
	if (n == 0)
		response.append(message);
	else
		response.append(message, n);
}
