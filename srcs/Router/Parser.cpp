#include <Parser.hpp>
#include <iostream>
#include <algorithm>

Parser::Parser(std::vector<t_token> const &tokens) : _tokens(tokens), _blocks(NULL)
{
    std::string dn[7] = {"listen", "server_name", "client_max_body_size", "root", "allowed_methods", "autoindex", "cgi_setup"};
    for (uint i = 0; i != 7; i++) {
        directiveNames.push_back(dn[i]);
    }
    // t_block_type type = BL_SERVER;
    parse(&_blocks, tokens, 0);
}

bool notSpace(t_token token)
{
    return (token.first != TOK_SP && token.first != TOK_RL);
}

int funNextNonSpTok(std::vector<t_token> const &tokens)
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
    }
        // Throw an error if you don't find
    return(0);
}

bool isDirective(t_token token, std::vector<std::string> const & directiveNames)
{
    return (token.first == TOK_WORD && std::find(directiveNames.begin(), directiveNames.end(), token.second) != directiveNames.end());
}

void    printVector(std::vector<t_token> tokens){
    uint    i = 0;
    for (std::vector<t_token>::iterator  it = tokens.begin(); it != tokens.end(); it++) {
        std::cout << i << " " << it->first << " " << it->second << std::endl;
        i++;
    }
}

bool    tokensAreNotWords(std::vector<t_token> const & tokens) {
    size_t    i = 0;
    while(i < tokens.size() && tokens[i].first != TOK_WORD){
        i++;
    }
    return(i == tokens.size());
}

// void Parser::parse(std::pair<uint, uint> limits, t_block_type type, std::vector<t_token> const &tokens)
void Parser::parse(Block **blockServer, std::vector<t_token>  const & tokens, uint j)
{
    // std::cout << "-----------------TOKENS------------------" << std::endl;
    // printVector(tokens);
    if (j > 1 || tokens.size() == 0){
        return ;
    }
    uint    st3 = 0;
    std::vector<t_token>    stt3;
    // std::cout << "j: " << j << " size: " << tokens.size() << std::endl;
    if (tokens.size() || !tokensAreNotWords(tokens))
    {
        uint nextNonSpTok = funNextNonSpTok(tokens);
        std::vector<t_token>    subToken1(tokens.begin() +nextNonSpTok + 1, tokens.end());
        // std::cout << "-----------------SUBTOKEN1------------------" << std::endl;
        // printVector(subToken1);
        uint nextNextNonSpTok = funNextNonSpTok(subToken1);
        if (tokens[nextNonSpTok].second == "server" && subToken1[nextNextNonSpTok].first == TOK_BR_OP)
        {
            std::cout << "I entered here" << std::endl;
            std::ostringstream oss;
            oss << j;
            // if (!_blocks)
            // {
                *blockServer = new Block(BL_SERVER, "server_" + oss.str());
            // }
            // else
            // {
                // _blocks->addSibling(new Block(BL_SERVER, "server_" + oss.str()));
            // }
            std::vector<t_token>    subToken2(subToken1.begin() +nextNextNonSpTok + 1, subToken1.end());
            uint    i = 0;
            uint    clIndexBr = closingIndexBracket(subToken2);
            while (i < closingIndexBracket(subToken2)) {
                if (isDirective(subToken2[i], directiveNames)){
                    std::vector<t_token>    subToken3(subToken2.begin() + i + 1, subToken2.end());
                    st3 = funNextNonSpTok(subToken3);
                    (*blockServer)->addDirective(std::make_pair(subToken2[i].second, subToken3[st3].second));
                    stt3 = subToken3;
                    i += st3 + 1 + 1;
                    }
                else {
                    i++;
                }
            }

            std::vector<t_token>    subToken4(subToken2.begin() + clIndexBr + 1, subToken2.end());

            parse((*blockServer)->getSiblingAddress(), subToken4, j + 1);
       }
    }
}

void    Parser::printBlocks() const{
   Block    *tmp = _blocks;
   while (tmp){
    tmp->printBlock();
    tmp = tmp->getSibling();
   }
}
