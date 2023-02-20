#include <Ressource.hpp>

//	file_path of the CGI script to be defined
CGI::CGI(Connexion *conn, std::string file_path, std::string cgi_path) :	Ressource(conn)
{
	int		pipe_to_CGI[2];
	int		pipe_to_host[2];
	char	*args[] = {const_cast<char*>(cgi_path.c_str()), const_cast<char*>(file_path.c_str())};

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

		setenv("REQUEST_METHOD", conn->getRequest().request_line.method.c_str(), 1);
		setenv("QUERY_STRING", conn->getRequest().request_line.path.c_str(), 1);

		execve(cgi_path.c_str(), args, NULL);
	}
	else
	{
		close(pipe_to_host[READ]);
		close(pipe_to_CGI[WRITE]);
		fd_read = pipe_to_CGI[READ];
		set_nonblocking(fd_read);
		fd_write = pipe_to_host[WRITE];
		set_nonblocking(fd_write);
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
	size_t ret = ::read(fd_read, buffer, BUFFER_SIZE);

	if (ret == -1)
	{
		close(fd_read);
		return conn->setError("Error while reading CGI response", 500);
	}
	if (!ret)
		return closed();
	if (ret == 0)
		is_EOF = true;
	conn->append_response(buffer, ret);
	return IOEvent();
}

IOEvent	CGI::write()
{
	size_t ret = ::write(fd_write, conn->getRequest().body.c_str(),
		conn->getRequest().body.size());

	if (ret <= 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return IOEvent();
		else
		{
			close(fd_write);
			return conn->setError("Error while writing to CGI", 500);
		}
	}
}

IOEvent	CGI::closed()
{
	return IOEvent(FAIL, this, "CGI::closed() called");
}
