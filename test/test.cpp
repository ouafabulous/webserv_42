#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <vector>
#include <cstdlib>

std::string	cgi_path = "./php-cgi/script.py";
int			fd_read, fd_write;
char		*args[] = { const_cast<char*>(cgi_path.c_str()), NULL };

void	pipe_communication(int &fd_read, int &fd_write, pid_t &pid)
{
	std::string	buffer;
	int	pipe_in[2];
	int	pipe_out[2];

	if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
	{
		perror("pipe");
		exit(1);
	}
	pid = fork();
	if (pid < 0)
	{
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);
		perror("fork");
		exit(1);
	}
	else if (pid == 0)
	{
		close(pipe_out[1]);
		close(pipe_in[0]);
		dup2(pipe_out[0], STDIN_FILENO);
		dup2(pipe_in[1], STDOUT_FILENO);
		close(pipe_out[0]);
		close(pipe_in[1]);

		if (!cgi_path.empty())
			execve(cgi_path.c_str(), args, NULL);
		else
		{
			std::cerr << "Error: CGI path is empty." << std::endl;
			exit(1);
		}
	}
	else
	{
		char buffer[1024];

		close(pipe_out[0]);
		close(pipe_in[1]);
		fd_read = pipe_in[0];
		fd_write = pipe_out[1];
		while (read(fd_write, buffer, 1024) != 0)
			std::cout << buffer << std::endl;
	}
}

int main()
{
	pid_t	pid;
	pipe_communication(fd_read, fd_write, pid);
	return 0;
}
