#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8081

int main(int argc, char **av) {
	int		server_fd, new_socket, valread;
	char	buffer[1024] = {0};
	struct	sockaddr_in address;
	int		addrlen = sizeof(address);

	    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

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
	if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

	//valread = read(new_socket, buffer, 1024);
	while (1) {
		if (recv(new_socket, buffer, 1024, MSG_DONTWAIT) > 0) {
			printf("%s\n", buffer);
		}
	}
	printf("%s\n", buffer);
	printf("end message\n");

}
