#include <Ressource.hpp>

Ressource::Ressource(Connexion *conn) : conn(conn), is_EOF(false), fd_read(-1), fd_write(-1) {}

CGI::CGI(Connexion *conn, std::string cgi_path) :	conn(conn), is_EOF(false)
{
	int		pipe_in[2];
	int		pipe_out[2];

	if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
		throw std::runtime_error("CGI::CGI() Pipe failed.");

	pid_t	pid = fork();

	if (pid < 0)
	{
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);
		throw std::runtime_error("CGI::CGI() Fork failed");
	}
	else if (pid == 0)
	{
		close(pipe_out[1]);
		close(pipe_in[0]);
		dup2(pipe_out[0], STDIN_FILENO);	// child closes write, redirects stdin to read
		dup2(pipe_in[1], STDOUT_FILENO);	// child closes read, redirects stdout to write
		close(pipe_out[0]);
		close(pipe_in[1]);

		setenv("REQUEST_METHOD", conn->t_http_message.t_request_line.method.c_str(), 1);
		setenv("QUERY_STRING", conn->t_http_message.body.c_str(), 1); // no idea if c_str() will work with a vector

		execve(cgi_path.c_str(), NULL, NULL); // not good, need to pass env instead of NULL
	}
	else
	{
		close(pipe_out[0]);
		close(pipe_in[1]);
		fd_read = pipe_in[0];
		fd_write = pipe_out[1];
	}
}

CGI::~CGI()
{
	epoll_util(EPOLL_CTL_DEL, fd_read, EPOLLIN);
	epoll_util(EPOLL_CTL_DEL, fd_write, EPOLLOUT)
	close(fd_read);
	close(fd_write);
}

void	CGI::read()
{
	char	buffer[BUFFER_SIZE];
	size_t	ret;

	ret = recv(fd_read, buffer, BUFFER_SIZE)
	reponse.append(buffer, ret);
	if (ret == 0)
		is_EOF = true;
}

void	CGI::write()
{
	if (send(fd_write, conn->t_http_message.body[0],
		conn->t_http_message.body.size(), MSG_DONTWAIT) == -1)
		throw std::runtime_error("CGI::write() failed.");
}

void	CGI::closed()
{
	throw std::runtime_error("CGI::closed() called");
}

t_fd	CGI::fdDelete() { return conn; }
