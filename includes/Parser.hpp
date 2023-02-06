#pragma once
#ifndef LEXER_HPP
# define LEXER_HPP

#include <vector>
#include <string>
#include "Type.hpp"

#include "Block.hpp"

class Block;

class Parser {
    public:
        Parser(std::vector<t_token>	const & tokens);
        ~Parser(){};
        std::pair<unsigned int, unsigned int>   getBlockLimits() const;
        // void                                    parse(Block *current_block, std::pair<uint, uint> limits, t_block_type type, std::vector<t_token>	const & tokens);
        void                                    parse(std::pair<uint, uint> limits, std::vector<t_token>	const & tokens);
        uint                                    getNextTokIndex(std::string const &tokens);

    private:
        std::vector<t_token>	          _tokens;
        Block                             *_blocks;
        std::vector<std::string>    directiveNames;
};

#endif