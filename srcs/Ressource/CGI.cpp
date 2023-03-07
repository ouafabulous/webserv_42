#include <Ressource.hpp>
void ignore_signal(int signal)
{
	(void)signal;
}

CGI::CGI(Connexion *conn, t_cgiInfo cgiInfo) : Ressource(conn)
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
		execve(cgiInfo._executable.c_str(), args, setCgiEnv(cgiInfo._filePath, cgiInfo._queryString, conn));
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

char **CGI::setCgiEnv(std::string filePath, std::string queryString, Connexion *conn)
{
	std::vector<std::string> args_vector;

	if (conn->getRequest().request_line.methodVerbose == "POST" && conn->getRequest().content_length > 0)
	{
		std::stringstream ss;
		ss << conn->getRequest().content_length;
		std::string content_length_str = ss.str();
		args_vector.push_back("CONTENT_LENGTH=" + content_length_str);
	}
	args_vector.push_back("CONTENT_TYPE=" + get_mime(filePath));
	args_vector.push_back("GATEWAY_INTERFACE=CGI/1.1");
	args_vector.push_back("QUERY_STRING=" + queryString);
	args_vector.push_back("REMOTE_ADDR=" + conn->client_ip_addr);
	args_vector.push_back("REQUEST_METHOD=" + conn->getRequest().request_line.methodVerbose);
	args_vector.push_back("SCRIPT_NAME=" + filePath);
	args_vector.push_back("SERVER_NAME=" + conn->getRouteCgi()->getAttributes().server_name[0]);
	std::stringstream ss;
	ss << conn->getRouteCgi()->getAttributes().port;
	args_vector.push_back("SERVER_PORT=" + ss.str());
	args_vector.push_back("SERVER_PROTOCOL=HTTP/1.1");
	args_vector.push_back("SERVER_SOFTWARE=webserv/1.0");
	args_vector.push_back("HTTP_COOKIE=" + conn->getRequest().header_fields.find("Cookie")->second);
	//args_vector.push_back("REMOTE_HOST=" + conn->client_hostname); // either domain name or NULL (not mandatory for python)

	// https://www.rfc-editor.org/rfc/rfc3875

	char **env = new char*[args_vector.size() + 1];
	size_t i = 0;
	for (std::vector<std::string>::iterator it = args_vector.begin(); it != args_vector.end(); it++)
		env[i++] = const_cast<char *>(it->c_str());
	env[i] = NULL;

	return (env);
}
