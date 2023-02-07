#include "Block.hpp"

void	Block::addSibling(Block *sibling){
	Block	*tmp = this;
	while (tmp->_sibling){
		tmp = tmp->_sibling;
	}
	tmp->_sibling = sibling;
}
