#include <Ressource.hpp>

void fill_env_variables(Connexion *conn, t_cgiInfo cgiInfo, std::vector<std::string> &args_vector)
{
	// https://www.rfc-editor.org/rfc/rfc3875
	std::map<std::string, std::string>::const_iterator it;

	std::string script_full_path(realpath(cgiInfo._filePath.c_str(), NULL));

	if (conn->getRequest().request_line.methodVerbose == "POST" && conn->getRequest().content_length > 0)
	{
		std::stringstream ss;
		ss << conn->getRequest().content_length;
		args_vector.push_back("CONTENT_LENGTH=" + ss.str());
		Logger::error << args_vector.back() << std::endl;
	}

	// args_vector.push_back("CONTENT_TYPE=" + get_mime(cgiInfo._filePath));
	std::string content_type = "text/plain";
	it = conn->getRequest().header_fields.find("Content-Type");
	if (it != conn->getRequest().header_fields.end())
		content_type = it->second;
	args_vector.push_back("CONTENT_TYPE=" + content_type);
	Logger::error << args_vector.back() << std::endl;
	args_vector.push_back("REDIRECT_STATUS=200");
	args_vector.push_back("PATH_INFO=" + script_full_path);
	args_vector.push_back("SCRIPT_NAME=" + script_full_path);
	args_vector.push_back("SCRIPT_FILENAME=" + script_full_path);
	Logger::error << args_vector.back() << std::endl;
	args_vector.push_back("GATEWAY_INTERFACE=CGI/1.1");
	args_vector.push_back("QUERY_STRING=" + cgiInfo._queryString);
	args_vector.push_back("REMOTE_ADDR=" + conn->client_ip_addr);
	args_vector.push_back("REQUEST_METHOD=" + conn->getRequest().request_line.methodVerbose);
	std::string host = conn->getRouteCgi()->getAttributes().server_name[0];
	it = conn->getRequest().header_fields.find("Host");
	if (it != conn->getRequest().header_fields.end())
		host = it->second;
	args_vector.push_back("SERVER_NAME=" + host);
	std::stringstream ss;
	ss << conn->getRouteCgi()->getAttributes().port;
	args_vector.push_back("SERVER_PORT=" + ss.str());
	args_vector.push_back("SERVER_PROTOCOL=HTTP/1.1");
	args_vector.push_back("SERVER_SOFTWARE=webserv/1.0");
	it = conn->getRequest().header_fields.find("Set-Cookie");
	if (it != conn->getRequest().header_fields.end())
	{
		std::string cookieval = it->second;
		args_vector.push_back("HTTP_COOKIE=" + cookieval);
	}
	it = conn->getRequest().header_fields.find("Cookie");
	if (it != conn->getRequest().header_fields.end())
		args_vector.push_back("HTTP_COOKIE=" + it->second);
}

void ignore_signal(int signal)
{
	(void)signal;
}

CGI::CGI(Connexion *conn, t_cgiInfo cgiInfo) : Ressource(conn), content_length(-1), header_received(false)
{
	int pipe_to_CGI[2];
	int pipe_to_host[2];
	char *args[] = {const_cast<char *>(cgiInfo._executable.c_str()), const_cast<char *>(cgiInfo._filePath.c_str()), NULL};

	Logger::info << conn->client_ip_addr << " - CGI args" << cgiInfo._executable << " " << cgiInfo._filePath << std::endl;
	if (pipe(pipe_to_CGI) == -1)
		throw IOExcept("CGI::CGI() pipe_to_CGI failed.", 500);
	if (pipe(pipe_to_host) == -1)
	{
		close(pipe_to_CGI[READ]);
		close(pipe_to_CGI[WRITE]);
		throw IOExcept("CGI::CGI() pipe_to_host failed.", 500);
	}

	if (poll_util(POLL_CTL_ADD, pipe_to_CGI[READ], this, POLLIN | POLLHUP | POLLRDHUP))
		throw IOExcept("CGI::CGI() poll_util failed", 500);
	if (poll_util(POLL_CTL_ADD, pipe_to_host[WRITE], this, POLLOUT | POLLRDHUP))
		throw IOExcept("CGI::CGI() poll_util failed", 500);

	pid = fork();

	if (pid < 0)
	{
		close(pipe_to_CGI[READ]);
		close(pipe_to_CGI[WRITE]);
		close(pipe_to_host[READ]);
		close(pipe_to_host[WRITE]);
		throw IOExcept("CGI::CGI() Fork failed", 500);
	}
	else if (pid == 0)
	{
		close(pipe_to_host[WRITE]);
		close(pipe_to_CGI[READ]);
		if (dup2(pipe_to_host[READ], STDIN_FILENO) == -1)
		{
			close(pipe_to_host[READ]);
			close(pipe_to_CGI[WRITE]);
			throw IOExcept("CGI::CGI() dup2 for pipe_to_host failed", 500);
		}
		if (dup2(pipe_to_CGI[WRITE], STDOUT_FILENO) == -1)
		{
			close(pipe_to_host[READ]);
			close(pipe_to_CGI[WRITE]);
			throw IOExcept("CGI::CGI() dup2 for pipe_to_CGI failed", 500);
		}
		close(pipe_to_host[READ]);
		close(pipe_to_CGI[WRITE]);

		signal(SIGINT, ignore_signal);

		std::vector<std::string> args_vector;
		fill_env_variables(conn, cgiInfo, args_vector);

		char *env[args_vector.size() + 1];
		size_t i = 0;
		for (std::vector<std::string>::iterator it = args_vector.begin(); it != args_vector.end(); it++)
			env[i++] = const_cast<char *>(it->c_str());
		env[i] = NULL;

		execve(cgiInfo._executable.c_str(), args, env);
		Logger::error << "CGI::CGI() execve failed" << std::endl;
		exit(1);
	}
	else
	{
		close(pipe_to_host[READ]);
		close(pipe_to_CGI[WRITE]);
		fd_read = pipe_to_CGI[READ];
		if (set_nonblocking(fd_read))
			throw IOExcept("Error setting the file to non-blocking", 500);
		fd_write = pipe_to_host[WRITE];
		if (set_nonblocking(fd_write))
			throw IOExcept("Error setting the file to non-blocking", 500);
	}
}

CGI::~CGI()
{
	poll_util(POLL_CTL_DEL, fd_read, this, POLLIN);
	poll_util(POLL_CTL_DEL, fd_write, this, POLLOUT);
	kill(pid, SIGTERM);
	if (close(fd_read) == -1 || close(fd_write) == -1)
		throw IOExcept("Error closing the file", 500);
}

bool CGI::read_header()
{
	size_t header_end = raw_header.find("\r\n\r\n");
	if (header_end)
	{
		size_t pos = raw_header.find("Status: ");
		if (pos != std::string::npos)
		{
			std::stringstream response;
			response << "HTTP/1.1 " << raw_header.substr(pos + 8, raw_header.find("\r", pos));
			conn->pushResponse(response.str());
		}
		pos = raw_header.find("Content-Length: ");
		if (pos != std::string::npos)
		{
			if (!(std::stringstream(raw_header.substr(pos + 16)) >> content_length))
				return NOK;
			conn->pushResponse(raw_header.substr(0, header_end + 4));
			raw_body.append(raw_header.substr(header_end + 4));
		}
		else
			conn->pushResponse(raw_header);
		header_received = true;
	}
	else if(raw_header.size() > MAX_HEADER_SIZE)
		return NOK;
	return OK;
}

IOEvent CGI::read()
{
	int ret = ::read(fd_read, buffer, BUFFER_SIZE);

	Logger::debug << "read from CGI" << std::endl;

	if (ret == -1)
		return conn->setError("Error reading the file", 500);
	if (ret == 0 || std::string(buffer, ret).find('\0') != std::string::npos)
	{
		conn->setRespEnd();
		poll_util(POLL_CTL_MOD, fd_read, this, 0);
	}
	if (ret > 0)
	{
		if (!header_received)
		{
			raw_header.append(buffer, ret);
			if (read_header())
				conn->setError("read_header() failed", 500);
		}
		else if (content_length < 0)
			conn->pushResponse(buffer, ret);
		if (content_length > 0) {
			raw_body.append(buffer, ret);
			if (static_cast<int>(raw_body.size()) >= content_length) {
				conn->pushResponse(raw_body.substr(0, content_length));
				conn->setRespEnd();
				poll_util(POLL_CTL_MOD, fd_read, this, 0);
			}
		}
	}

	return IOEvent();
}

IOEvent CGI::write()
{
	if (conn->getRequest().body.empty())
		return IOEvent();

	Logger::debug << "write to CGI" << std::endl;

	if (conn->getBodyParsed() == true && conn->getRequest().body.empty()) {
		::write(fd_write, "", 0);
		poll_util(POLL_CTL_MOD, fd_write, this, 0);
	}

	std::string		str = conn->getRequest().body.front();
	Logger::debug << str << std::endl;

	int ret = ::write(fd_write, str.c_str(), str.size());

	if (ret <= 0)
		return conn->setError("Error while writing to CGI", 500);
	if (ret < static_cast<int>(str.size()))
		conn->getRequest().body.front() = str.substr(ret);
	else if (ret == static_cast<int>(str.size())) {
		conn->getRequest().body.pop();
	}
	return IOEvent();
}
