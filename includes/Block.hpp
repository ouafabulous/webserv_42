#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "Type.hpp"
#include <iostream>

class Block{
    public:
        Block(t_block_type type, std::string name) : _type(type), _name(name){};
        virtual ~Block(){};

        void    addDirective(directive dir) {_directives.push_back(dir);};
        void    printBlock();
        // Block   **getChildAddress();


    protected:
        t_block_type            _type;
        std::string             _name;
        std::vector<directive>  _directives;
};

class BlockLocation : public Block {
    public:
    BlockLocation(std::string name, std::string locationvalue) : Block(BL_LOCATION, name) {_locationValue = locationvalue;};

    std::string getName() const;
    std::string getLocationValue() const;

    private:
    std::string         _locationValue;    
};

class BlockServer : public Block {
    public:
    BlockServer(std::string name) : Block(BL_SERVER, name) {};
    
    size_t      getNumberChild() const;
    void        addChild(BlockLocation *child);
    void        printBlock();

    BlockServer   **getSiblingAddress();
    void    addSibling(BlockServer *sibling);
    BlockServer   *getSibling();

    private:
    std::vector<BlockLocation *>      _childs;
    BlockServer                   *_sibling;
};

#endif
