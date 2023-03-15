#pragma once
#include <iostream>

class IndexBufferInfo
{
private:
	uint32_t _nb_elem;

public:
	IndexBufferInfo();

	void setNbElem(uint32_t nb_elem);

	const uint32_t getNbElem();
};

