#include <Parser.hpp>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <vector>
#include <map>
#include <Errors.hpp>
// #include <cstdint>
#include <algorithm>
#include <climits>
#include <string.h>
#include <stdexcept>
#include <string>
#include <Directive.hpp>
#include "Type.hpp"

Parser::~Parser()
{
    BlockServer *tmp1 = _blocks;
    BlockServer *tmp2 = _blocks;
    while (tmp1)
    {
        std::vector<BlockLocation *> const &childs = tmp1->getChilds();
        for (std::vector<BlockLocation *>::const_iterator it = childs.begin(); it != childs.end(); it++)
        {
            delete (*it);
        }
        tmp2 = tmp1;
        tmp1 = tmp1->getSibling();
        delete (tmp2);
    }
}

Parser::Parser(TokenList const &tokens) : _tokens(tokens), _blocks(NULL)
{
    std::string dn[10] = {"listen", "server_name", "client_max_body_size", "allowed_methods", "autoindex", "cgi_setup", "root", "error_files", "return", "index"};
    for (uint i = 0; i != 11; i++)
    {
        directiveNames.push_back(dn[i]);
    }
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

void printVector(TokenList tokens)
{
    uint i = 0;
    for (TokenList::iterator it = tokens.begin(); it != tokens.end(); it++)
    {
        std::cout << i << " " << it->first << " " << it->second << std::endl;
        i++;
    }
}

uint closingIndexBracket(TokenList const &tokens, uint j)
{
    uint i = j;
    uint br = 1;
    for (TokenList::const_iterator it = tokens.begin() + j; it != tokens.end(); it++)
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
    return (0);
}

bool isDirective(t_token token, std::vector<std::string> const &directiveNames)
{
    return (token.first == TOK_WORD && std::find(directiveNames.begin(), directiveNames.end(), token.second) != directiveNames.end());
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

Directive parseDirective(TokenList const &tokens, uint &startIndex)
{
    // Create a new directive with the name of the current token
    std::string directiveName = tokens[startIndex].second;
    Directive directive(directiveName);

    // Loop through the tokens until the end of the directive (semicolon)
    startIndex++;
    while (tokens[startIndex].first != TOK_SC)
    {
        // If the current token is a word, add it as a value of the directive
        if (tokens[startIndex].first == TOK_WORD)
        {
            directive.addDirectiveValue(tokens[startIndex].second);
        }
        startIndex++;
    }
    return directive;
}

void Parser::parse(BlockServer **block, TokenList const &tokens, uint serverNumber)
{
    if (tokens.empty() || !tokensAreNotWords(tokens))
        return;

    uint i = findNextNonSpTok(tokens, 0);
    if (tokens[i].second != "server")
        throw std::runtime_error("Error while parsing the conf_file!");

    i = findNextNonSpTok(tokens, i + 1);
    if (tokens[i].first != TOK_BR_OP)
        throw std::runtime_error("Error while parsing the conf_file!");

    std::ostringstream oss;
    oss << serverNumber;
    *block = new BlockServer("server_" + oss.str());
    i++;
    uint clServerBrIndex = closingIndexBracket(tokens, i);
    // std::cout << "i value: " << i << ", clServerBrIndex value: " << clServerBrIndex << std::endl;
    while (i < clServerBrIndex)
    {
        if (isDirective(tokens[i], directiveNames))
        {
            Directive directive = parseDirective(tokens, i);
            (*block)->addDirective(directive);
        }
        else if (tokens[i].second == "location")
        {
            // std::cout << "I entered here 2" << std::endl;
            uint firstNonSpTokIndex = findNextNonSpTok(tokens, i + 1);
            uint secondNonSpTokIndex = findNextNonSpTok(tokens, firstNonSpTokIndex + 1);
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
                        Directive directive = parseDirective(tokens, i);
                        locationBlock->addDirective(directive);
                    }
                    else
                        i++; // normalement c'est une erreur pcque what would it be if it is not a directive?
                }
                static_cast<BlockServer *>(*block)->addChild(locationBlock);
                i = clLocationBrIndex + 1;
            }
        }
        else
            throw std::runtime_error("Error while parsing the conf_file!");
    }
    // keep doing it recursively
    TokenList subToken(tokens.begin() + clServerBrIndex + 1, tokens.end());
    parse((*block)->getSiblingAddress(), subToken, serverNumber + 1);
    return;
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

BlockServer *Parser::getBlock() const
{
    return (_blocks);
}