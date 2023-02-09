#include "Block.hpp"


void	BlockServer::addSibling(BlockServer *sibling){
	BlockServer	*tmp = this;
	while (tmp->_sibling){
		tmp = tmp->_sibling;
	}
	tmp->_sibling = sibling;
}

void	BlockServer::printBlock() {
	std::cout << "\033[1;31m" << _name << "\033[0m" << std::endl;
	for (std::vector<directive>::iterator  it = _directives.begin(); it != _directives.end(); it++){
		std::cout << it->first << " " << it->second << std::endl;
	}
	for (std::vector<BlockLocation *>::iterator it = _childs.begin(); it != _childs.end(); it++) {
		std::cout <<"\033[3;100;30m" << (*it)->getName() << " " << (*it)->getLocationValue() << "\033[0m\n" << std::endl; 
	}
}

BlockServer *BlockServer::getSibling(){
	return(_sibling);
}
BlockServer **BlockServer::getSiblingAddress(){
	return(&_sibling);
}

size_t	BlockServer::getNumberChild() const {
	return (_childs.size());
}

void	BlockServer::addChild(BlockLocation *child){
	_childs.push_back(child);
}

std::string	BlockLocation::getName() const {
	return (_name);
}

std::string	BlockLocation::getLocationValue() const {
	return	(_locationValue);
}