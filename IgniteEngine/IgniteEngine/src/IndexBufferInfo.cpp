#include "IndexBufferInfo.h"

IndexBufferInfo::IndexBufferInfo() :
	_nb_elem{}
{
	;
}

void IndexBufferInfo::setNbElem(uint32_t nb_elem) {
	_nb_elem = nb_elem;
}


void IndexBufferInfo::setIndexType(VkIndexType index_type) {
	_index_type = index_type;
}

const uint32_t IndexBufferInfo::getNbElem() {
	return _nb_elem;
}

const VkIndexType IndexBufferInfo::getIndexType() const {
	return _index_type;
}