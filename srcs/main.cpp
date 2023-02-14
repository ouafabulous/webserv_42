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
	(void)ac;
	(void)av;

	signal(SIGPIPE, handle_sigpipe);
	signal(SIGINT, handle_sigint);
	if (ac == 2)
	{
		std::ifstream file(av[1]);
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string big_buffer = buffer.str();

		Lexer	Lex(big_buffer);
		Lex.fillTokens();
		// Lex.printTokens();
		try{
			Parser	Parse(Lex.getTokens());
			Parse.printBlocks();
		}
		 catch (const std::runtime_error &e)
		{       
			std::cout << "ERROR WHILEPARSING" << std::endl;                                                         // specify the exception type
            //throw std::runtime_error("Error: " + std::string(e.what())); // re-throw the exception with a modified message
        }
		// // Lexer	Lex(big_buffer);
		// // Lex.fillTokens();
		// // Lex.printTokens();
		

	// try {
		// Logger::setLevel(DEBUG);
		// Server my_server("");
		// my_server.routine();
	// }
	// catch(const std::exception& e) {
		// Logger::error << e.what();
	}
}
