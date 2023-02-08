#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "Type.hpp"
#include <iostream>

class Block{
    public:
        Block(t_block_type type, std::string name) : _type(type), _name(name), _sibling(NULL), _child(NULL) {};
        ~Block(){};

        void    addSibling(Block *sibling);
        void    addDirective(directive dir) {_directives.push_back(dir);};
        void    printBlock();
        Block   *getSibling();
        Block   **getSiblingAddress();


    private:
        t_block_type            _type;
        std::string             _name;
        std::vector<directive>  _directives;
        Block                   *_sibling;
        Block                   *_child;
};

#endif
