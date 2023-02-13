#include <Lexer.hpp>
#include <iostream>

// HELPER FUNC

std::string escape_string(const std::string &str) {
  std::string result;
  for (size_t i = 0; i < str.length(); i++)
  {
    switch (str[i]) {
      case '\n':
        result += "\\n";
        break;
      case '\r':
        result += "\\r";
        break;
      case '\t':
        result += "\\t";
        break;
      case '\"':
        result += "\\\"";
        break;
      case '\'':
        result += "\\\'";
        break;
      case '\\':
        result += "\\\\";
        break;
      default:
        result += str[i];
        break;
    }
  }
  return result;
}

// END HELPER

Lexer::Lexer(const std::string confile): _confile(confile)
{
	// GET_CHR_CLASS INIT
	for (int c = '\0'; c <= ' '; ++c)
		get_chr_class[static_cast<int>(c)] = CHR_SP;
	get_chr_class[static_cast<int>(';')] = CHR_SC;
	for (int c = '!'; c <= ':'; ++c)
		get_chr_class[static_cast<int>(c)] = CHR_WORD;
	for (int c = '<'; c <= 'z'; ++c)
	get_chr_class[static_cast<int>(c)] = CHR_WORD;
	get_chr_class[static_cast<int>('{')] = CHR_BR_OP;
	get_chr_class[static_cast<int>('|')] = CHR_WORD;
	get_chr_class[static_cast<int>('}')] = CHR_BR_CL;
	get_chr_class[static_cast<int>('~')] = CHR_WORD;

	// GET_TOK_TYPE INIT
	get_tok_type[CHR_SP] = TOK_SP;
	get_tok_type[CHR_WORD] = TOK_WORD;
	get_tok_type[CHR_RL] = TOK_RL;
	get_tok_type[CHR_SC] = TOK_SC;
	get_tok_type[CHR_BR_OP] = TOK_BR_OP;
	get_tok_type[CHR_BR_CL] = TOK_BR_CL;

	// TOK_RULES INIT
	for (size_t x = 0; x < TOK_S_MAX; x++) {
		for (size_t y = 0; y < CHR_MAX; y++)
		{
			tok_rules[x][y] = 0;
		}
	}
	tok_rules[TOK_WORD][CHR_WORD] = 1;
	tok_rules[TOK_SP][CHR_SP] = 1;
	tok_rules[TOK_RL][CHR_RL] = 1;
	tok_rules[TOK_SC][CHR_SC] = 1;
	tok_rules[TOK_BR_OP][CHR_BR_OP] = 1;
	tok_rules[TOK_BR_CL][CHR_BR_CL] = 1;
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
		tok_type = get_tok_type[get_chr_class[static_cast<t_s_tok>(_confile[0])]];
		i = 1;
		while (i < _confile.length() && tok_rules[tok_type][get_chr_class[static_cast<t_s_tok>(_confile[i])]])
			i++;
		_tokens.push_back(t_token(tok_type, _confile.substr(0, i)));
		_confile = _confile.substr(i, _confile.length());
	}
}

TokenList const & Lexer::getTokens() const
{
	return (_tokens);
}

void				Lexer::printTokens() {
	for (TokenList::const_iterator it = _tokens.begin(); it != _tokens.end(); it++)
		std::cout << *it << std::endl;
}

std::ostream&	operator<<(std::ostream &out, const t_token &c)
{
	const static std::string	text[6] = {"TOK_WORD", "TOK_SP\t", "TOK_RL\t", "TOK_SC\t", "TOK_BR_OP\t", "TOK_BR_CL\t"};

    out << text[c.first] << ":\t\"" << escape_string(c.second) << "\"";
    return out;
}
