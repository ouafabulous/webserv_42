#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "Type.hpp"
#include <iostream>

class Block{
    public:
        Block(t_block_type type, std::string name) : _type(type), _name(name){};
        virtual ~Block(){};

        void                addDirective(directive dir) {_directives.push_back(dir);};
        virtual void        printBlock();
        std::string         getName() const;
        // Block   **getChildAddress();
        std::vector<directive> const    &getDirectives() const;

    protected:
        t_block_type            _type;
        std::string             _name;
        std::vector<directive>  _directives;
};

class BlockLocation : public Block {
    public:
    BlockLocation(std::string name, std::string locationvalue) : Block(BL_LOCATION, name) {_locationValue = locationvalue;};

    std::string     getLocationValue() const;
    // void                            printBlock();

    private:
    std::string                     _locationValue;    
    std::vector<directive>          _directives;
};

class BlockServer : public Block {
    public:
    BlockServer(std::string name) : Block(BL_SERVER, name) {_sibling = NULL;};
    
    size_t          getNumberChild() const;
    void            addChild(BlockLocation *child);
    // void            printBlock();
    BlockServer     **getSiblingAddress();
    void            addSibling(BlockServer *sibling);
    BlockServer     *getSibling() const;
    std::vector<BlockLocation *> const & getChilds() const;


    private:
    std::vector<BlockLocation *>    _childs;
    BlockServer                     *_sibling;
    std::vector<directive>          _directives;
};

#endif
