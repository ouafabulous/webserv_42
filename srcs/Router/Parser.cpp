#include <Parser.hpp>
#include <iostream>
#include <algorithm>


Parser::Parser(TokenList const &tokens) : _tokens(tokens), _blocks(NULL)
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

int funNextNonSpTok(TokenList const &tokens)
{
    TokenList::const_iterator tokNotSp = find_if(tokens.begin(), tokens.end(), notSpace);
    // Throw error if there is none
    return (tokNotSp - tokens.begin());
}

uint closingIndexBracket(TokenList &tokens)
{
    uint i = 0;
    uint br = 1;
    for (TokenList::iterator it = tokens.begin(); it != tokens.end(); it++)
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

void    printVector(TokenList tokens){
    uint    i = 0;
    for (TokenList::iterator  it = tokens.begin(); it != tokens.end(); it++) {
        std::cout << i << " " << it->first << " " << it->second << std::endl;
        i++;
    }
}

bool    tokensAreNotWords(TokenList const & tokens) {
    size_t    i = 0;
    while(i < tokens.size() && tokens[i].first != TOK_WORD){
        i++;
    }
    return(i == tokens.size());
}

TokenList    subVectorFrom(TokenList originalTokens, uint index){
    TokenList   toReturn(originalToken.begin() + index, originalToken.end());
    return(toReturn);
}

TokenList       Parser::parse_utils()    

void            Parser::parse(Block **blockServer, TokenList  const & tokens, uint j)
{
    if (j > 1 || tokens.size() == 0){
        return ;
    }
    uint    st3 = 0;
    TokenList    stt3;
    if (tokens.size() || !tokensAreNotWords(tokens))
    {
        uint nextNonSpTok = funNextNonSpTok(tokens);
        TokenList    subToken1 = subVectorFrom(tokens, nextNonSpTok + 1);
        uint nextNextNonSpTok = funNextNonSpTok(subToken1);
        if (tokens[nextNonSpTok].second == "server" && subToken1[nextNextNonSpTok].first == TOK_BR_OP)
        {
            std::ostringstream oss;
            oss << j;
            *blockServer = new BlockServer("server_" + oss.str());
            TokenList    subToken2(subToken1.begin() +nextNextNonSpTok + 1, subToken1.end());
            uint    i = 0;
            uint    clIndexBr = closingIndexBracket(subToken2);
            while (i < closingIndexBracket(subToken2)) {
                if (isDirective(subToken2[i], directiveNames)){
                    TokenList    subToken3(subToken2.begin() + i + 1, subToken2.end());
                    st3 = funNextNonSpTok(subToken3);
                    (*blockServer)->addDirective(std::make_pair(subToken2[i].second, subToken3[st3].second));
                    stt3 = subToken3;
                    i += st3 + 1 + 1;
                    }
                else {
                    i++;
                }
            }
            TokenList    subToken4(subToken2.begin() + clIndexBr + 1, subToken2.end());
            parse((*blockServer)->getSiblingAddress(), subToken4, j + 1);
            return ;
       }
       TokenList subToken2 = subVectorFrom(nextNextNonSpTok + 1);
       uint nextNextNextNonSpTok = funNextNonSpTok(subToken2);
       if (tokens[nextNonSpTok].second == "location" && subToken1[nextNextNonSpTok].first == TOK_WORD && subToken2[nextNextNextNonSpTok].first == TOK_BR_OP) 
       {

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
