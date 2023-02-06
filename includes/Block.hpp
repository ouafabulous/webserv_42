#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "Type.hpp"

class Block{
    public:
        Block(t_block_type type) : _type(type), _name(NULL), _sibling(NULL), _child(NULL) {};
        ~Block(){};

    private:
        t_block_type            _type;
        std::string             _name;
        std::vector<directive>  _directives;
        Block                   *_sibling;
        Block                   *_child;
};

#endif