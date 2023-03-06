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
	if (filename.length() >= extension.length())
	{
		if (filename.compare(filename.length() - extension.length(), extension.length(), extension))
			throw std::runtime_error("Configuration file must have .conf extension\n");
	}
	else
		throw std::runtime_error("Configuration file must have .conf extension\n");
}

bool containsExtendedChars(const std::string& str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (*it < 0)
            return true;
    }
    return false;
}

std::string checkConfigFile(int ac, char *av[])
{
	std::string config_file;

	if (ac == 1)
		config_file = "./config_file/default.conf";
	else if (ac == 2)
		config_file = av[1];
	else								// 1- check arguments !
		throw std::runtime_error("Usage: ./webserv or ./webserv config_file\n");
	if (!fileExists(config_file.c_str()))				// 2- check if the file exists
		throw std::runtime_error("Config file not found!\n");
	hasConfExtension(config_file);			// 3- check if the config file has .conf extension !
	if (!checkPermissions(config_file.c_str(), R_OK)) // 4- if the conf file has read permissions !
		throw std::runtime_error("Read permission is required for the config file!\n");
	if (containsExtendedChars(config_file.c_str()))	// 5- if the conf file contains extended ascii chars !
		throw std::runtime_error("Ascii extended is not supported in config file!\n");

	// read file
	std::ifstream file(config_file.c_str());
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string big_buffer = buffer.str();

	if (!big_buffer.size())
		throw std::runtime_error("Config file empty!\n"); //6- check if conf file is empty
	if (containsExtendedChars(big_buffer))				  //7- check if the config file contains extended ascii chars
		throw std::runtime_error("Extended ascii characters in config file are not supported!\n");
	return (big_buffer);
}

int main(int ac, char *av[])
{
	Logger::setLevel(INFO);

	// signal(SIGPIPE, handle_sigpipe);
	signal(SIGINT, handle_sigint);
	try
	{
		// basic checkings of the conf file + conversion to std::string
		std::string	confFile = checkConfigFile(ac, av);
		//  lexing
		Lexer lex(confFile);
		lex.fillTokens();

		// parsing
		Parser parse(lex.getTokens());

		// server declaration
		Server my_server(parse);

		// routine
		my_server.routine();
	}
	catch (const std::exception &e)
	{
		Logger::error << e.what();
	}
}
