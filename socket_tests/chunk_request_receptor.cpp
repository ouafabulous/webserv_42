#include <iostream>
#include <string>
#include <sstream>
#include <vector>



int main() {
    // Assume the socket has been created and connected to a server
    int socket_fd = ...;

    // Receive the headers and check for the "Transfer-Encoding: chunked" header
    std::string headers;
    while (true) {
        char buffer[4096];
        int bytes_received = recv(socket_fd, buffer, sizeof(buffer), 0);
        headers.append(buffer, bytes_received);
        if (headers.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    }
    if (headers.find("Transfer-Encoding: chunked") == std::string::npos) {
        std::cerr << "Error: Not a chunked request" << std::endl;
        return 1;
    }

    // Receive and unchunk the request body
    std::stringstream request_body;
    while (true) {
        // Receive the next chunk size
        std::string chunk_size_str;
        while (true) {
            char buffer[1];
            int bytes_received = recv(socket_fd, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) {
                std::cerr << "Error: Failed to receive chunk size" << std::endl;
                return 1;
            }
            chunk_size_str.append(buffer, bytes_received);
            if (chunk_size_str.back() == '\r') {
                recv(socket_fd, buffer, sizeof(buffer), 0);
                break;
            }
        }

        // Convert the chunk size from hexadecimal to integer
        int chunk_size = std::stoi(chunk_size_str, 0, 16);
        if (chunk_size == 0) {
            break;
        }

        // Receive the chunk data
        std::vector<char> chunk_data(chunk_size);
        int bytes_received = recv(socket_fd, chunk_data.data(), chunk_size, 0);
        if (bytes_received != chunk_size) {
            std::cerr << "Error: Failed to receive chunk data" << std::endl;
            return 1;
        }
        recv(socket_fd, buffer, 2, 0);

        // Append the chunk data to the request body
        request_body.write(chunk_data.data(), chunk_size);
    }

    // Print the unchunked request body
    std::cout << request_body.str() << std::endl;

    return 0;
}
