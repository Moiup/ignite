#include "IndexBufferInfo.h"

IndexBufferInfo::IndexBufferInfo() :
	_nb_elem{}
{
	;
}

void IndexBufferInfo::setNbElem(uint32_t nb_elem) {
	_nb_elem = nb_elem;
}

const uint32_t IndexBufferInfo::getNbElem() {
	return _nb_elem;
}