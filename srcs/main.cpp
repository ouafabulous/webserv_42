#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "Router.hpp"
#include "Type.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "signal.h"
#include "stdlib.h"

void handle_sigint(int signal)
{
	(void)signal;
	std::cout << "\n";
	throw std::runtime_error("Shutdown server\n");
}

void hasConfExtension(std::string const &filename)
{
	std::string extension = ".conf";
	if (filename.length() >= extension.length()){
		if (filename.compare(filename.length() - extension.length(), extension.length(), extension))
			throw std::runtime_error("Configuration file must have .conf extension\n");
	}
	else
		throw std::runtime_error("Configuration file must have .conf extension\n");
}

int main(int ac, char *av[])
{
	Logger::setLevel(DEBUG);

	// signal(SIGPIPE, handle_sigpipe);
	signal(SIGINT, handle_sigint);
	try
	{
		//basic checkings of the conf file
		if (ac != 2) 									//1- if more we haven't received one argument !
			throw std::runtime_error("Usage: ./webserv config_file\n");
		hasConfExtension(av[1]);						//2- if the config file has .conf extension !
		if (!checkPermissions(av[1], R_OK))				//3- if the conf file has read permissions !
			throw std::runtime_error("Read permission is required for the config file!\n");
		
		// read file
		std::ifstream file(av[1]);
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string big_buffer = buffer.str();

		// lexing
		Lexer lex(big_buffer);
		lex.fillTokens();

		// parsing
		Parser parse(lex.getTokens());
		
		//server declaration
		Server my_server(parse);
		
		//routine
		my_server.routine();
	}
	catch (const std::exception &e){
		Logger::error << e.what();
	}
}