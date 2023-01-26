#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <sys/epoll.h>
#include <map>
#include <fcntl.h>
#define BUFFER_SIZE 4096
#define BACKLOG 3
#define PORT 8081
#include <vector>

// class Basic_route;

// typedef std::map<std::pair<in_addr_t, in_port_t>, std::map<std::string, Basic_route>> router_map;

// for(router_map::const_iterator it = router.begin(), it != router.end(), it++) {
// 	socks.push_back()
// }

// router[{127.0.0.1, 20}]["google.com"]

class Error_404 : public std::exception {
	public:
		virtual const char* what() const throw() {
			return ("HTTP/1.1 404 OK\nContent-Type:text/html\nContent-Length: 49\n\n<html><body><h1>File not found</h1></body></html>");
		}
};

typedef	int	fd;

int	setnonblocking(int fd) {
	if (fd < 0)
		return false;
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return false;
	return (fcntl(fd, F_SETFL,flags | O_NONBLOCK));
}

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

int main(void)
{
	int							server_fd;
	std::map<int, std::string>	my_conns;
	struct sockaddr_in			address;
	int							addrlen = sizeof(address);
	epoll_event					tmp_epoll_event;
	epoll_event					events[BACKLOG];
	char						buffer[BUFFER_SIZE];
	int							epoll_wait_return;
	int							recv_size;
	fd							current_fd;
	std::string					hello = "HTTP/1.1 200 OK\nContent-Type:text/plain\nContent-Length: 12\n\nHello World!";
	std::vector<fd>				to_read;

	// call to http://localhost:8081/index.html

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	int	reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse)))
		perror("ERROR SO_REUSEADDR:");

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if(setnonblocking(server_fd) == -1)
		exit(-1);
// Pour les socket de
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	int epfd = epoll_create(10);
	tmp_epoll_event.data.fd = server_fd;
	tmp_epoll_event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &tmp_epoll_event) == -1) {
		perror("epoll_ctl");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		epoll_wait_return = epoll_wait(epfd, events, BACKLOG, 1);
		if (epoll_wait_return == -1) {
			perror("epoll_wait");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < epoll_wait_return; i++)
		{
			current_fd = events[i].data.fd;
			if (current_fd == server_fd)
			{
				if ((tmp_epoll_event.data.fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) > 0)
				{
					if (setnonblocking(tmp_epoll_event.data.fd) == -1)
						exit(-1);
					tmp_epoll_event.events = EPOLLIN;
					std::cout << tmp_epoll_event.data.fd << " is now connected" << std::endl;
					if (epoll_ctl(epfd, EPOLL_CTL_ADD, tmp_epoll_event.data.fd, &tmp_epoll_event) == -1)
						exit(250);
				}
			}
			else
			{
				if (events[i].events & EPOLLIN) {
					std::cout << "** EPOLLIN **" << std::endl;
					if ((recv_size = recv(current_fd, buffer, BUFFER_SIZE, MSG_DONTWAIT)) > 0) {
						buffer[recv_size -1] = 0;
						my_conns[current_fd].append(buffer);
					}
					if (ends_with(my_conns[current_fd], "\n\r")) {
						std::cout << my_conns[current_fd] << std::endl;
						std::cout << "request ok" << std::endl;
						tmp_epoll_event.data.fd = current_fd;
						tmp_epoll_event.events = EPOLLOUT | EPOLLET;
						epoll_ctl(epfd, EPOLL_CTL_MOD, current_fd, &tmp_epoll_event);
					}
				}
				if (events[i].events & EPOLLOUT) {
					std::cout << "** EPOLLOUT **" << std::endl;
					try
					{
						throw Error_404();
						if (send(current_fd, hello.c_str(), hello.length(), MSG_DONTWAIT) > 0)
							std::cout << "response ok\n=== === ===" << std::endl;
					}
					catch(std::exception& e)
					{
						std::string error = e.what();
						if (send(current_fd, error.c_str(), error.length(), MSG_DONTWAIT) > 0)
							std::cout << "response ok\n=== === ===" << std::endl;
						std::cerr << e.what() << '\n';
					}
					if (epoll_ctl(epfd, EPOLL_CTL_DEL, current_fd, NULL) == -1)
						perror("epoll_ctl");
					if (close(current_fd) == -1)
						perror("close");
					my_conns.erase(current_fd);
				}
			}
		}
	}
}
