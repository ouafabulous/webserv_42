#include <Parser.hpp>
#include <iostream>
#include <algorithm>

Parser::Parser(TokenList const &tokens) : _tokens(tokens), _blocks(NULL)
{
    std::string dn[7] = {"listen", "server_name", "client_max_body_size", "root", "allowed_methods", "autoindex", "cgi_setup"};
    for (uint i = 0; i != 7; i++)
    {
        directiveNames.push_back(dn[i]);
    }
    // t_block_type type = BL_SERVER;
    parse(&_blocks, tokens, 0);
}

bool notSpace(t_token token)
{
    return (token.first != TOK_SP && token.first != TOK_RL);
}

int findNextNonSpTok(TokenList const &tokens, uint i)
{
    TokenList::const_iterator tokNotSp = find_if(tokens.begin() + i, tokens.end(), notSpace);
    // Throw error if there is none
    return (tokNotSp - tokens.begin());
}

uint closingIndexBracket(TokenList const &tokens, uint j)
{
    uint i = 0;
    uint br = 1;
    TokenList subVector(tokens.begin() + j, tokens.end());

    for (TokenList::const_iterator it = tokens.begin(); it != tokens.end(); it++)
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
    return (0);
}

bool isDirective(t_token token, std::vector<std::string> const &directiveNames)
{
    return (token.first == TOK_WORD && std::find(directiveNames.begin(), directiveNames.end(), token.second) != directiveNames.end());
}

void printVector(TokenList tokens)
{
    uint i = 0;
    for (TokenList::iterator it = tokens.begin(); it != tokens.end(); it++)
    {
        std::cout << i << " " << it->first << " " << it->second << std::endl;
        i++;
    }
}

bool tokensAreNotWords(TokenList const &tokens)
{
    size_t i = 0;
    while (i < tokens.size() && tokens[i].first != TOK_WORD)
    {
        i++;
    }
    return (i == tokens.size());
}

TokenList subVectorFrom(TokenList originalTokens, uint index)
{
    TokenList toReturn(originalTokens.begin() + index, originalTokens.end());
    return (toReturn);
}

// a function that returns the nextNonSp token from a given index.

void Parser::parse(BlockServer **block, TokenList const &tokens, uint serverNumber)
{
    if (tokens.size() || !tokensAreNotWords(tokens))
    {
        uint firstNonSpTokIndex = findNextNonSpTok(tokens, 0); // the first non space token in the tokens given
        uint secondNonSpTokIndex = findNextNonSpTok(tokens, firstNonSpTokIndex + 1);
        if (tokens[firstNonSpTokIndex].second == "server" && tokens[secondNonSpTokIndex].first == TOK_BR_OP)
        {
            std::ostringstream oss;
            oss << serverNumber;
            *block = new BlockServer("server_" + oss.str());
            uint i = secondNonSpTokIndex + 1;
            uint clServerBrIndex = closingIndexBracket(tokens, i);
            while (i < clServerBrIndex)
            {
                if (isDirective(tokens[i], directiveNames))
                {
                    std::string directiveName = tokens[i].second;
                    firstNonSpTokIndex = findNextNonSpTok(tokens, i + 1);
                    t_token directiveValueTok = tokens[firstNonSpTokIndex];
                    if (directiveValueTok.first == TOK_WORD)
                    {
                        (*block)->addDirective(std::make_pair(directiveName, directiveValueTok.second));
                    }
                    i = firstNonSpTokIndex + 1;
                }
                else if (tokens[i].second == "location")
                {
                    firstNonSpTokIndex = findNextNonSpTok(tokens, i + 1);
                    secondNonSpTokIndex = findNextNonSpTok(tokens, firstNonSpTokIndex + 1);
                    if (tokens[firstNonSpTokIndex].first == TOK_WORD && tokens[secondNonSpTokIndex].first == TOK_BR_OP)
                    {
                        oss << static_cast<BlockServer *>(*block)->getNumberChild();
                        std::string locationName = "Location_" + oss.str();
                        std::string locationValue = tokens[firstNonSpTokIndex].second;
                        BlockLocation *locationBlock = new BlockLocation(locationName, locationValue);
                        i = secondNonSpTokIndex + 1;
                        uint clLocationBrIndex = closingIndexBracket(tokens, i);
                        while (i < clLocationBrIndex)
                        {
                            if (isDirective(tokens[i], directiveNames))
                            {
                                std::string directiveName = tokens[i].second;
                                firstNonSpTokIndex = findNextNonSpTok(tokens, i + 1);
                                t_token directiveValueTok = tokens[firstNonSpTokIndex];
                                if (directiveValueTok.first == TOK_WORD)
                                {
                                    (*block)->addDirective(std::make_pair(directiveName, directiveValueTok.second));
                                }
                                i = firstNonSpTokIndex + 1;
                            }
                            else
                            {
                                i++; // normalement c'est une erreur pcque what would it be if it is not a directive?
                            }
                        }
                        static_cast<BlockServer *>(*block)->addChild(locationBlock);
                        i = clLocationBrIndex + 1;
                    }
                    else
                    {
                        i++;
                    }
                }
                TokenList subToken(tokens.begin() + clServerBrIndex + 1, tokens.end());
                parse((*block)->getSiblingAddress(), subToken, serverNumber + 1);
                return;
            }
        }
    }
}

void Parser::printBlocks() const
{
    BlockServer *tmp = _blocks;
    while (tmp)
    {
        tmp->printBlock();
        tmp = tmp->getSibling();
    }
}
