#include "Type.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "signal.h"
#include "stdlib.h"
#include "Router.hpp"

void handle_sigpipe(int signal) {
	(void)signal;
	Logger::error << "Caught SIGPIPE signal, ignoring it." << std::endl;
}
void handle_sigint(int signal) {
	(void)signal;
	std::cout << "\n";
	throw std::runtime_error("Shutdown server");
}

int main(int ac, char *av[])
{
	Logger::setLevel(DEBUG);

	// signal(SIGPIPE, handle_sigpipe);
	signal(SIGINT, handle_sigint);
	if (ac != 2) {
		Logger::error << "configuration file path required" << std::endl;
		return (1);
	}

	try {
		// read file
		std::ifstream file(av[1]);
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string big_buffer = buffer.str();

		// lexing
		Lexer	lex(big_buffer);
		lex.fillTokens();
		lex.printTokens();
		// Parser	parse(lex.getTokens());
		// parse.printBlocks();
		// Router	router(parse);
		// router.printRoutes();
		// Server my_server(parse);
		// my_server.routine();
	}
	catch(const std::exception& e) {
		Logger::error << e.what();
	}
}
