#include <Ressource.hpp>

//	file_path of the CGI script to be defined
CGI::CGI(Connexion *conn, t_cgiInfo cgiInfo) :	Ressource(conn)
{
	int		pipe_to_CGI[2];
	int		pipe_to_host[2];
	//bytes_read = 0;
	char	*args[] = {const_cast<char*>(cgiInfo._executable.c_str()), const_cast<char*>(cgiInfo._filePath.c_str())};

	if (pipe(pipe_to_CGI) == -1)
		throw std::runtime_error("CGI::CGI() pipe_to_CGI failed.");
	if (pipe(pipe_to_host) == -1)
	{
		close(pipe_to_CGI[READ]);
		close(pipe_to_CGI[WRITE]);
		throw std::runtime_error("CGI::CGI() pipe_to_host failed.");
	}

	if (poll_util(POLL_CTL_ADD, pipe_to_CGI[READ], this, POLLIN | POLLHUP | POLLRDHUP)
		&& poll_util(POLL_CTL_ADD, pipe_to_host[WRITE], this, POLLOUT | POLLRDHUP))
		throw std::runtime_error("CGI::CGI() poll_util failed");

	pid_t pid = fork();

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

		char *env[] = {
			//const_cast<char*>("REQUEST_METHOD=" + conn->getRequest().request_line.methodVerbose),
			//const_cast<char*>("QUERY_STRING=" + cgiInfo._queryString),
			NULL
		};


		execve(cgiInfo._executable.c_str(), args, env);
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
	if (close(fd_read) == -1 || close(fd_write) == -1)
	{
		conn->setError("Error closing the file", 500);
		throw std::runtime_error("PostStaticFile::~PostStaticFile() Close failed");
	}
}

IOEvent CGI::read()
{
	//// not handling chuncked request yet ?
	//int ret = ::read(fd_read, buffer, BUFFER_SIZE);

	//if (ret == -1)
	//{
	//	close(fd_read);
	//	return conn->setError("Error while reading CGI response", 500);
	//}
	//if (!ret)
	//	return closed();
	//if (ret == 0)
	//	conn->setRespEnd();
	//conn->pushResponse(buffer, ret);
	//return IOEvent();
	int	ret = ::read(fd_read, buffer, BUFFER_SIZE);

	Logger::debug << "read from file" << std::endl;

	if (ret == -1)
		return conn->setError("Error reading the file", 500);
	if (ret < BUFFER_SIZE) {
		conn->setRespEnd();
		poll_util(POLL_CTL_MOD, fd_read, this, 0);
	}
	if (ret)
		conn->pushResponse(buffer, ret);
	return IOEvent();
}

IOEvent CGI::write()
{
	if (conn->getRequest().body.empty())
		return IOEvent();

	int ret = ::write(fd_write, conn->getRequest().body.c_str(),
					conn->getRequest().body.size());

	if (!ret)
	{
		conn->setRespEnd();
		return IOEvent();
	}

	if (ret < 0)
		return conn->setError("Error while writing to CGI", 500);
	return IOEvent();
}

IOEvent CGI::closed()
{
	return conn->setError("An error happend while running CGI binary", 500);
}
