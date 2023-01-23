#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#define BUFFER_SIZE 1
#define BACKLOG 3
#define PORT 8081

#include <vector>

typedef struct s_conn {
	int				socket_fd;
	std::string		result;
}	t_conn;

int main(int argc, char **av) {
	int		server_fd, valread;
	t_conn	tmp_conn;
	std::vector<t_conn>		my_conns;
	struct	sockaddr_in address;
	int		addrlen = sizeof(address);
	char	buffer[BUFFER_SIZE];

	    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
	int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse)))
		perror("ERROR SO_REUSEADDR:");

	address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
	if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
		// int flags = fcntl(server_fd, F_GETFL, 0);
		// flags = flags | O_NONBLOCK;
		// fcntl(server_fd, F_SETFL, flags);
		fcntl(server_fd, F_SETFL, O_NONBLOCK);
	while (1) {
		// std::cout << "// Reading from accept()" << std::endl;
		if ((tmp_conn.socket_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) > 0) {
			std::cout << tmp_conn.socket_fd << " is now connected" << std::endl;
			my_conns.push_back(tmp_conn);
			// perror("accept");
			// exit(EXIT_FAILURE);
		}

	for(std::vector<t_conn>::iterator it = my_conns.begin(); it != my_conns.end(); it++) {
		if (recv(it->socket_fd, buffer, BUFFER_SIZE, MSG_DONTWAIT) > 0) {
				it->result.append(buffer);
				std::cout << it->result << std::endl;
				usleep(1000000);
			}
		}
	}
	// printf("%s\n", tmp_conn.buffer);
	// printf("end message\n");

}
