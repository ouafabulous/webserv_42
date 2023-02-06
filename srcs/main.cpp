#include "Type.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "Lexer.hpp"



int main(int ac, char *av[])
{
	if (ac == 2)
	{
		std::ifstream file(av[1]);
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string big_buffer = buffer.str();

		Lexer	Lex(big_buffer);
		Lex.fillTokens();
		Lex.printTokens();
	}
}