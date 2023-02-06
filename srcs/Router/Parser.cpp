#include <Parser.hpp>
#include <iostream>
#include <algorithm>

Parser::Parser(std::vector<t_token> const &tokens)
{
    std::string dn[7] = {"listen", "server_name", "client_max_body_size", "root", "allowed_methods", "autoindex", "cgi_setup"};
    for (uint i = 0; i != 7; i++) {
        directiveNames.push_back(dn[i]);
    }
    std::pair<uint, uint> limits = std::make_pair(0, 1);
    t_block_type type = BL_SERVER;
    parse(limits, tokens);
}

bool notSpace(t_token token)
{
    return (token.first != TOK_SP && token.first != TOK_RL);
}

int NextNonSpTok(std::vector<t_token> const &tokens)
{
    std::vector<t_token>::const_iterator tokNotSp = find_if(tokens.begin(), tokens.end(), notSpace);
    // Throw error if there is none
    return (tokNotSp - tokens.begin());
}

uint closingIndexBracket(std::vector<t_token> &tokens)
{
    uint i = 0;
    uint br = 1;
    for (std::vector<t_token>::iterator it = tokens.begin(); it != tokens.end(); it++)
    {
        i++;
        if (it->first == TOK_BR_OP)
        {
            br++;
        }
        else if (it->first == TOK_BR_CL)
        {
            br--;
            if (br == 0)
            {
                return (i);
            }
        }
        // Throw an error
    }
}

bool isDirective(t_token token, std::vector<std::string> const & directiveNames)
{
    return (token.first == TOK_WORD && std::find(directiveNames.begin(), directiveNames.end(), token.second) != directiveNames.end());
}

// void Parser::parse(std::pair<uint, uint> limits, t_block_type type, std::vector<t_token> const &tokens)
void Parser::parse(std::pair<uint, uint> limits, std::vector<t_token> const &tokens)
{
{
    std::vector<std::string>    subTokens(tokens.begin() + limits.first, tokens.begin() + limits.second);
    while (subTokens.length())
    {
        uint nextNonSpTok = NextNonSpTok(tokens);
        uint nextNextNonSpTok = NextNonSpTok(tokens.substr(nextNonSpTok + 1));
        if (tokens[NextNonSpTok] == "server" && tokens[nextNextNonSpTok].first == TOK_BR)
        {
            Block   *currentBlock = new Block;
            if (!*_blocks)
            {
                _blocks = currentBlock;
            }
            else
            {
                _blocks->next = currentBlock;
            }
            for (uint i = 0; i != closingIndexBracket(tokens[nextNextNonSpTok + 1]))
            {
                if (isDirective(tokens[i])){
                    currentBlock->
                }
            }
        }
    }
}
