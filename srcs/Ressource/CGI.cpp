#include <Ressource.hpp>

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

		std::vector<std::string>	args_vector;
		args_vector.push_back("REQUEST_METHOD=" + conn->getRequest().request_line.methodVerbose);
		args_vector.push_back("QUERY_STRING=" + cgiInfo._queryString);

		// https://www.rfc-editor.org/rfc/rfc3875

		// Parse and add these values to env for CGI script
		// AUTH_TYPE = "" | auth-scheme
		// CONTENT_LENGTH = "" | 1*DIGIT or NULL
		// CONTENT_TYPE = "" | media-type
		// GATEWAY_INTERFACE = CGI/1.1
		// PATH_INFO = "" | path
		// PATH_TRANSLATED = ??
		// REMOTE_ADDR = hostnumber
		// REMOTE_HOST = "" | hostname | hostnumber
		// REMOTE_USER = *TEXT
		// REQUEST_METHOD = method
		// SCRIPT_NAME = path
		// SERVER_NAME = hostname
		// SERVER_PORT = port
		// SERVER_PROTOCOL = protocol
		// SERVER_SOFTWARE = servername "/" version

		char *env[args_vector.size() + 1];
		size_t i = 0;
		for(std::vector<std::string>::iterator it = args_vector.begin(); it != args_vector.end(); it++)
			env[i++] = const_cast<char*>(it->c_str());
		env[i] = NULL;

		try
		{
			execve(cgiInfo._executable.c_str(), args, env);
		}
		catch (const std::exception &e)
		{
			Logger::error << "CGI::CGI() execve failed: " << e.what() << std::endl;
			exit(1);
		}
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
