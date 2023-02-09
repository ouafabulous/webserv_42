#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

std::string cgi_path = "/usr/bin/python3";
std::string script_path = "./php-cgi/script.py";
char *args[] = {const_cast<char *>(cgi_path.c_str()), const_cast<char *>(script_path.c_str())};

void pipe_communication(char **env)
{
	pid_t pid;
	int fd_read, fd_write;
	std::string buffer;
	int pipe_host_to_cgi[2];
	int pipe_cgi_to_host[2];
	int wstatus;

	if (pipe(pipe_host_to_cgi) == -1 || pipe(pipe_cgi_to_host) == -1)
	{
		perror("pipe");
		exit(1);
	}
	pid = fork();
	if (pid < 0)
	{
		close(pipe_host_to_cgi[READ]);
		close(pipe_host_to_cgi[WRITE]);
		close(pipe_cgi_to_host[READ]);
		close(pipe_cgi_to_host[WRITE]);
		perror("fork");
		exit(1);
	}
	else if (pid == 0)
	{
		close(pipe_host_to_cgi[WRITE]);
		close(pipe_cgi_to_host[READ]);
		dup2(pipe_host_to_cgi[READ], STDIN_FILENO);
		dup2(pipe_cgi_to_host[WRITE], STDOUT_FILENO);
		close(pipe_host_to_cgi[READ]);
		close(pipe_cgi_to_host[WRITE]);

		if (!cgi_path.empty())
			execve(cgi_path.c_str(), args, env);
		else
		{
			std::cerr << "Error: CGI path is empty." << std::endl;
			exit(1);
		}
	}
	else
	{
		char buffer[1024];

		close(pipe_host_to_cgi[READ]);
		close(pipe_cgi_to_host[WRITE]);
		fd_write = pipe_host_to_cgi[WRITE];
		fd_read = pipe_cgi_to_host[READ];
		// std::string request = "POST /path HTTP/1.1\r\n"
		// 					  "Host: example.com\r\n"
		// 					  "Content-Type: application/x-www-form-urlencoded\r\n"
		// 					  "Content-Length: 25\r\n"
		// 					  "\r\n"
		std::string request_body = "first_name=Maxime&last_name=Riaud\r\n";
		write(fd_write, request_body.c_str(), request_body.length());

		while(read(fd_read, buffer, 1024) > 0)
			std::cout << buffer << std::endl;

		wait(&wstatus);
		close(fd_read);
		close(fd_write);
	}
}

int main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	pipe_communication(env);
	return 0;
}
