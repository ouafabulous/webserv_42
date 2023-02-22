#include "Block.hpp"

void BlockServer::addSibling(BlockServer *sibling)
{
	BlockServer *tmp = this;
	while (tmp->_sibling)
	{
		tmp = tmp->_sibling;
	}
	tmp->_sibling = sibling;
}

// std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& vec)
// {
//     os << "[";
//     for (std::vector<std::string>::const_iterator iterVecs = vec.begin(); iterVecs != vec.end(); iterVecs++) {
//         os << *iterVecs << ", ";
//     }
//     os << "]";
//     return os;
// }

void Block::printBlock()
{
	if (_type == BL_SERVER)
	{
		std::cout << "\033[1;31m" << getName() << "\033[0m" << std::endl;
		// std::vector<BlockLocation *> const &childs = static_cast<BlockServer *>(this)->getChilds();
		// for (std::vector<BlockLocation *>::const_iterator itLocation = childs.begin(); itLocation != childs.end(); itLocation++)
		// {
			// (*itLocation)->printBlock();
		// }
		// for (std::vector<Directive>::iterator  itDir = _directives.begin(); itDir != _directives.end(); itDir++){
		// std::cout << "\033[1;31m" << itDir->getDirectiveName() << " " << itDir->getDirectiveValues() << "\033[0m"  << std::endl;
		// }
	}
	else if (_type == BL_LOCATION)
	{
		std::cout << "\033[3;100;30m" << getName() << " " << static_cast<BlockLocation *>(this)->getLocationValue() << "\033[0m\n"
				  << std::endl;
		// for (std::vector<Directive>::iterator  itDir = _directives.begin(); itDir != _directives.end(); itDir++){
		// std::cout << "\033[3;100;30m" << itDir->first << " " << itDir->second << "\033[0m\n"  << std::endl;
		// }
	}
}

BlockServer *BlockServer::getSibling() const
{
	return (_sibling);
}
BlockServer **BlockServer::getSiblingAddress()
{
	return (&_sibling);
}

size_t BlockServer::getNumberChild() const
{
	return (_childs.size());
}

void BlockServer::addChild(BlockLocation *child)
{
	_childs.push_back(child);
}

std::string Block::getName() const
{
	return (_name);
}

std::string BlockLocation::getLocationValue() const
{
	return (_locationValue);
}

std::vector<BlockLocation *> const &BlockServer::getChilds() const
{
	return (_childs);
}

std::vector<Directive> const &Block::getDirectives() const
{
	return (_directives);
}