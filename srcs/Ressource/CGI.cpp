#include <Ressource.hpp>

CGI::CGI(Connexion *conn, std::string cgi_path) :	conn(conn),
													is_EOF(false)
{
	int		pipe_to_CGI[2];
	int		pipe_to_host[2];
	char	*args[] = {const_cast<char*>(cgi_path.c_str()), const_cast<char*>script_path};

	if (pipe(pipe_to_CGI) == -1)
		throw std::runtime_error("CGI::CGI() pipe_to_CGI failed.");
	if (pipe(pipe_to_host) == -1)
	{
		close(pipe_to_CGI[READ]);
		close(pipe_to_CGI[WRITE]);
		throw std::runtime_error("CGI::CGI() pipe_to_host failed.");
	}

	pid_t	pid = fork();

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

		setenv("REQUEST_METHOD", conn->request.request_line.method.c_str(), 1);
		setenv("QUERY_STRING", conn->request.request_line.path.c_str(), 1);

		execve(cgi_path.c_str(), args, NULL);
	}
	else
	{
		close(pipe_to_host[READ]);
		close(pipe_to_CGI[WRITE]);
		fd_read = pipe_to_CGI[READ];
		fd_write = pipe_to_host[WRITE];
	}
}

CGI::~CGI()
{
	epoll_util(EPOLL_CTL_DEL, fd_read, this, EPOLLIN);
	epoll_util(EPOLL_CTL_DEL, fd_write, this, EPOLLOUT);
	close(fd_read);
	close(fd_write);
}

IOEvent	CGI::read()
{
	char	buffer[BUFFER_SIZE];
	size_t	ret;

	if (ret = recv(fd_read, buffer, BUFFER_SIZE, MSG_DONTWAIT) > 0)
		response.insert(response.end(), buffer, buffer + ret);
	if (ret == 0)
		is_EOF = true;
	return IOEvent(SUCCESS, this, "CGI::read() OK");
}

IOEvent	CGI::write()
{
	if (send(fd_write, conn->request.body[0],
		conn->request.body.size(), MSG_DONTWAIT) == -1)
		return IOEvent(FAIL, this, "CGI::write() failed.");
}

IOEvent	CGI::closed()
{
	return IOEvent(FAIL, this, "CGI::closed() called");
}


