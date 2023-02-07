#include "Type.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <Server.hpp>

#include <signal.h>
#include <stdlib.h>

void handle_sigpipe(int signal) {
	(void)signal;
	Logger::error << "Caught SIGPIPE signal, ignoring it." << std::endl;
}
void handle_sigint(int signal) {
	(void)signal;
	std::cout << "\n";
	Logger::info << "Shutdown server" << std::endl;
	for (Server::socket_set::const_iterator it = Server::socks.begin(); it != Server::socks.end(); it++)
		delete	*it;
	if (Server::epollfd != -1)
		close(Server::epollfd);
	exit(0);
}

int main(int ac, char *av[])
{
	(void)ac;
	(void)av;

	signal(SIGPIPE, handle_sigpipe);
	signal(SIGINT, handle_sigint);
	Logger::setLevel(INFO);
	// if (ac == 2)
	// {
	// 	std::ifstream file(av[1]);
	// 	std::stringstream buffer;
	// 	buffer << file.rdbuf();
	// 	std::string big_buffer = buffer.str();

		// Lexer	Lex(big_buffer);
		// Lex.fillTokens();
		// Lex.printTokens();

	Server my_server("");
	try {
		my_server.routine();
	}
	catch(const std::exception& e) {
		Logger::error << e.what();
	}
}
