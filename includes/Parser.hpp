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
        void                                    parse(TokenList	const & tokens, uint serverId);
        void                                    freeBlocks();
        uint                                    getNextTokIndex(std::string const &tokens);
        void                                    printBlocks() const;
        std::vector<BlockServer> const          &getBlockServers() const;

    private:
        TokenList	                            _tokens;
        std::vector<BlockServer>                _blockServers;
        std::vector<std::string>                directiveNames;
};

#endif
