#include "Lexer.hpp"
#include <iostream>

Lexer::Lexer(const std::string confile) : get_chr_class({CHR_WORD}),
										  get_tok_type({
											  [CHR_SP] = TOK_SP,
											  [CHR_WORD] = TOK_WORD,
											  [CHR_RL] = TOK_RL,
											  [CHR_SC] = TOK_SC,
											  [CHR_BR] = TOK_BR,
										  }),
										  tok_rules({
											  [TOK_WORD] = {[CHR_SP] = 0, [CHR_WORD] = 1, [CHR_RL] = 0, [CHR_SC] = 0, [CHR_BR] = 0},
											  [TOK_SP] = {[CHR_SP] = 1, [CHR_WORD] = 0, [CHR_RL] = 0, [CHR_SC] = 0, [CHR_BR] = 0},
											  [TOK_RL] = {[CHR_SP] = 0, [CHR_WORD] = 0, [CHR_RL] = 1, [CHR_SC] = 0, [CHR_BR] = 0},
											  [TOK_SC] = {[CHR_SP] = 0, [CHR_WORD] = 0, [CHR_RL] = 0, [CHR_SC] = 1, [CHR_BR] = 0},
											  [TOK_BR] = {[CHR_SP] = 0, [CHR_WORD] = 0, [CHR_RL] = 0, [CHR_SC] = 0, [CHR_BR] = 1},
										  }),
										  _confile{confile)
{
	// GET_CHR_CLASS INIT
	get_chr_class[' '] = CHR_SP;
	get_chr_class['\t'] = CHR_SP;
	get_chr_class[59] = CHR_SC;
	get_chr_class['}'] = CHR_BR;
	get_chr_class['{'] = CHR_BR;
	get_chr_class['|'] = CHR_WORD;
	get_chr_class['}'] = CHR_BR;
	get_chr_class['~'] = CHR_WORD;
}
Lexer::~Lexer()
{
	// std::cout << RED << "Appel du destructeur de Lexer\n" << RESET;
}

void Lexer::fillTokens()
{
	t_s_tok tok_type;
	size_t i;

	while (_confile.length())
	{
		tok_type = get_tok_type[get_chr_class[_confile[0]]];
		i = 1;
		while (i < _confile.length() && tok_rules[tok_type][get_chr_class[_confile[i]]])
			i++;
		_tokens.push_back(t_token(tok_type, _confile.substr(0, i)));
		_confile = _confile.substr(i, _confile.length());
	}
}

std::vector<t_token> Lexer::getTokens() const
{
	return (_tokens);
}

const std::string	convertToText(t_s_tok tok){
	const std::string	text[5] = {"TOK_SP", "TOK_WORD", "TOK_RL", "TOK_SC", "TOK_BR"};
	return(text[tok]);
}

void				Lexer::printTokens() {
	for (std::vector<t_token>::const_iterator it = _tokens.begin(); it != _tokens.end(); it++)
		std::cout << convertToText(it->first) << " " << it->second << std::endl;
}
