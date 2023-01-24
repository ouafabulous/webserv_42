#include <arpa/inet.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8081

int main(void)
{
    int sock = 0, client_fd;
    struct sockaddr_in serv_addr;
	std::string 	hello = "hello from client";
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
	int i = 0;
	// while (i < hello.length()) {
	// 	send(sock, hello.c_str() + i, 1, 0);
    //     i++;
	// }
	while (i < 100000000) {

		send(sock, "01234567890123456789", 20, 0);
		i++;
	}
	send(sock, "\n\r", 2, 0);

    close(client_fd);
    return 0;
}
