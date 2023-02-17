#pragma once
#ifndef PARSER_HPP
# define PARSER_HPP

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "Type.hpp"

#include "Block.hpp"

class Block;

class Parser {
    public:
        Parser(TokenList	const & tokens);
        ~Parser();
        std::pair<unsigned int, unsigned int>   getBlockLimits() const;
        // void                                    parse(Block *current_block, std::pair<uint, uint> limits, t_block_type type, TokenList	const & tokens);
        void                                    parse(BlockServer **block, TokenList	const & tokens, uint serverId);
        uint                                    getNextTokIndex(std::string const &tokens);
        void                                    printBlocks() const;
        BlockServer                             *getBlock() const;

    private:
        TokenList	                            _tokens;
        BlockServer                             *_blocks;
        std::vector<std::string>                directiveNames;
};

#endif
