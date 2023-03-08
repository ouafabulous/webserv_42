#include <Ressource.hpp>

void fill_env_variables(Connexion *conn, t_cgiInfo cgiInfo, std::vector<std::string> &args_vector)
{
	// https://www.rfc-editor.org/rfc/rfc3875
	std::map<std::string, std::string>::const_iterator it;

	if (conn->getRequest().request_line.methodVerbose == "POST" && conn->getRequest().content_length > 0)
	{
		std::stringstream ss;
		ss << conn->getRequest().content_length;
		args_vector.push_back("CONTENT_LENGTH=" + ss.str());
	}
	args_vector.push_back("CONTENT_TYPE=" + get_mime(cgiInfo._filePath));
	args_vector.push_back("GATEWAY_INTERFACE=CGI/1.1");
	args_vector.push_back("QUERY_STRING=" + cgiInfo._queryString);
	args_vector.push_back("REMOTE_ADDR=" + conn->client_ip_addr);
	args_vector.push_back("REQUEST_METHOD=" + conn->getRequest().request_line.methodVerbose);
	args_vector.push_back("SCRIPT_NAME=" + cgiInfo._filePath);
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
	std::string cookie;
	it = conn->getRequest().header_fields.find("Set-Cookie");
	if (it != conn->getRequest().header_fields.end())
	{
		cookie = it->second;
		args_vector.push_back("HTTP_COOKIE=" + cookie);
	}
}

void print_env_variables(char **env)
{
	for (int i = 0; env[i] != NULL; i++)
	{
		Logger::error << "Environment variable: " << env[i] << std::endl;
	}
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
		throw std::runtime_error("CGI::CGI() pipe_to_CGI failed.");
	if (pipe(pipe_to_host) == -1)
	{
		close(pipe_to_CGI[READ]);
		close(pipe_to_CGI[WRITE]);
		throw std::runtime_error("CGI::CGI() pipe_to_host failed.");
	}

	if (poll_util(POLL_CTL_ADD, pipe_to_CGI[READ], this, POLLIN | POLLHUP | POLLRDHUP) && poll_util(POLL_CTL_ADD, pipe_to_host[WRITE], this, POLLOUT | POLLRDHUP))
		throw std::runtime_error("CGI::CGI() poll_util failed");

	pid = fork();

	if (pid < 0)
	{
		close(pipe_to_CGI[READ]);
		close(pipe_to_CGI[WRITE]);
		close(pipe_to_host[READ]);
		close(pipe_to_host[WRITE]);
		throw std::runtime_error("CGI::CGI() Fork failed");
	}
	else if (pid == 0)
	{
		close(pipe_to_host[WRITE]);
		close(pipe_to_CGI[READ]);
		if (dup2(pipe_to_host[READ], STDIN_FILENO) == -1)
		{
			close(pipe_to_host[READ]);
			close(pipe_to_CGI[WRITE]);
			throw std::runtime_error("CGI::CGI() dup2 for pipe_to_host failed");
		}
		if (dup2(pipe_to_CGI[WRITE], STDOUT_FILENO) == -1)
		{
			close(pipe_to_host[READ]);
			close(pipe_to_CGI[WRITE]);
			throw std::runtime_error("CGI::CGI() dup2 for pipe_to_CGI failed");
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

		print_env_variables(env);
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
		{
			conn->setError("Error setting the file to non-blocking", 500);
			throw std::runtime_error("CGI::CGI() set_nonblocking failed");
		}
		fd_write = pipe_to_host[WRITE];
		if (set_nonblocking(fd_write))
		{
			conn->setError("Error setting the file to non-blocking", 500);
			throw std::runtime_error("CGI::CGI() set_nonblocking failed");
		}
	}
}

CGI::~CGI()
{
	poll_util(POLL_CTL_DEL, fd_read, this, POLLIN);
	poll_util(POLL_CTL_DEL, fd_write, this, POLLOUT);
	kill(pid, SIGTERM);
	if (close(fd_read) == -1 || close(fd_write) == -1)
	{
		conn->setError("Error closing the file", 500);
		throw std::runtime_error("PostStaticFile::~PostStaticFile() Close failed");
	}
}

bool CGI::read_header()
{
	size_t header_end = raw_header.find("\r\n\r\n");
	if (header_end)
	{
		size_t pos = raw_header.find("Content-Length: ");
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
				conn->setError("fkwe", 500);
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
