#include <Ressource.hpp>

CGI::CGI(Connexion *conn, std::string cgi_path) :	conn(conn), is_EOF(false)
{


	int		pipe_in[2];
	int		pipe_out[2];

	if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
		throw std::runtime_error("Pipe failed.");

	pid_t	pid = fork();

	if (pid < 0)
	{
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);
		throw std::runtime_error("Fork failed");
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

		execve(cgi_path.c_str(), NULL, NULL);
	}
	else
	{
		close(pipe_out[0]);
		close(pipe_in[1]);
		fd_read = pipe_in[0];
		fd_write = pipe_out[1];
	}
}

CGI::~CGI() {}
void	CGI::read() {}
void	CGI::write() {}
void	CGI::closed() {}
