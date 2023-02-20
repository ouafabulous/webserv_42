#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "Type.hpp"
#include "Directive.hpp"
#include <iostream>

class Directive;

class Block{
    public:
        Block(t_block_type type, std::string name) : _type(type), _name(name){};
        virtual ~Block(){};

        void                addDirective(Directive dir) {_directives.push_back(dir);};
        virtual void        printBlock();
        std::string         getName() const;
        std::vector<Directive> const    &getDirectives() const;

    protected:
        t_block_type            _type;
        std::string             _name;
        std::vector<Directive>  _directives;
};

class BlockLocation : public Block {
    public:
    BlockLocation(std::string name, std::string locationvalue) : Block(BL_LOCATION, name) {_locationValue = locationvalue;};

    std::string     getLocationValue() const;
    // void                            printBlock();

    private:
    std::string                     _locationValue;    
    std::vector<Directive>          _directives;
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
    std::vector<Directive>          _directives;
};

#endif
