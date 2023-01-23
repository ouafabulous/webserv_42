#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#define BUFFER_SIZE 4096
#define BACKLOG 3
#define PORT 8081

#include <map>

int	setnonblocking(int fd) {
	if (fd < 0)
		return false;
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return false;
	return (fcntl(fd, F_SETFL,flags | O_NONBLOCK));
}

int main(int argc, char **av)
{
	int server_fd, valread;
	int current_fd;
	std::map<int, std::string> my_conns;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	epoll_event tmp_epoll_event;
	char buffer[BUFFER_SIZE];
	epoll_event events[BACKLOG];
	int epoll_wait_return;
	int	tmp_recv;

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse)))
		perror("ERROR SO_REUSEADDR:");
	if(setnonblocking(server_fd) == -1)
		exit(-1);

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	int epfd = epoll_create(10);
	tmp_epoll_event.data.fd = server_fd;
	tmp_epoll_event.events = EPOLLIN;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &tmp_epoll_event) == -1)
		exit(250);
	while (1)
	{
		epoll_wait_return = epoll_wait(epfd, events, BACKLOG, 1);

		for (int i = 0; i < epoll_wait_return; i++)
		{
			if (events[i].data.fd == server_fd)
			{
				if ((tmp_epoll_event.data.fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) > 0)
				{
					if(setnonblocking(tmp_epoll_event.data.fd) == -1)
						exit(-1);
					std::cout << tmp_epoll_event.data.fd << " is now connected" << std::endl;
					if (epoll_ctl(epfd, EPOLL_CTL_ADD, tmp_epoll_event.data.fd, &tmp_epoll_event))
						exit(250);
				}
			}
			else
			{
				if ((tmp_recv = recv(events[i].data.fd, buffer, BUFFER_SIZE, MSG_DONTWAIT)) > 0) {
					// std::cout << tmp_recv << std::endl;
					buffer[tmp_recv] = 0;
					my_conns[events[i].data.fd].append(buffer);
					std::cout << my_conns[events[i].data.fd] << std::endl;
				}
			}
		}
	}
}
