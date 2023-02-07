#include "Block.hpp"


void	Block::addSibling(Block *sibling){
	Block	*tmp = this;
	while (tmp->_sibling){
		tmp = tmp->_sibling;
	}
	tmp->_sibling = sibling;
}

void	Block::printBlock() {
	std::cout << _name << std::endl;
	for (std::vector<directive>::iterator  it = _directives.begin(); it != _directives.end(); it++){
		std::cout << it->first << " " << it->second << std::endl;
	}
}

Block *Block::getSibling(){
	return(_sibling);
}
