#pragma once
#ifndef LEXER_HPP
# define LEXER_HPP

#include <vector>
#include <string>
#include "includes/Type.hpp"

class Lexer
{
	t_chr_class				get_chr_class[255];
	t_s_tok					get_tok_type[CHR_MAX];
	int 					tok_rules[TOK_S_MAX][CHR_MAX];
	std::string				_confile;
	std::vector<t_token>	_tokens;
	// Lexer(){};

public:
	Lexer(const std::string);
	// Lexer(const Lexer&);
	// void	operator=(const Lexer&);
	~Lexer();

	std::vector<t_token>	getTokens() const;

	void					fillTokens();
	void					printTokens();
	std::string				convertToText(t_s_tok tok);
};

#endif
