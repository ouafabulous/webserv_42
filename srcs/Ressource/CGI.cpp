#include <Ressource.hpp>

CGI::CGI(Connexion *conn, std::string cgi_path) :	conn(conn), response(), is_EOF(false)
{
	int		pipefd[2];
	
	if (pipe(pipefd) == -1)
		throw std::runtime_error("pipe");

	pid_t	pid = fork();

	if (pid < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		throw std::runtime_error("fork");
	}
	else if (pid == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		setenv("REQUEST_METHOD", conn->t_http_message.t_request_line.method.c_str(), 1);
		setenv("QUERY_STRING", conn->t_http_message.body.c_str(), 1);

		execve(cgi_path.c_str(), NULL, NULL);
	}
	else
	{
		close(pipefd[1]);
		fd_read = pipefd[0];
		fd_write = pipefd[1];
	}
}

CGI::~CGI() {}
void	CGI::read() {}
void	CGI::write() {}
void	CGI::closed() {}
