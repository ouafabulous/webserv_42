#pragma once
#ifndef LEXER_HPP
# define LEXER_HPP

#include <vector>
#include <string>
#include "Type.hpp"



class Block{
    public:
        Block(t_block_type type) : _type(type), sibling(NULL), child(NULL) {};
    private:
        t_block_type            _type;
        std::vector<directive>  directives;
        Block                   *sibling;
        Block                   *child;
};

class Parser {
    private:
        std::vector<std::vector >                    blocks;
        static const std::vector<std::string>       directiveNames;
};

#endif