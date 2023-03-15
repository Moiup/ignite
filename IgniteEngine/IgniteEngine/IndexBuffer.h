#pragma once

#include "Buffer.h"

class IndexBuffer : public Buffer
{
private:
	VkIndexType _index_type;
	uint32_t _nb_elem;

public:
	IndexBuffer();

	void create();

	void setIndexType(VkIndexType index_type);
	VkIndexType getIndexType();

	void setNbElem(uint32_t nb_elem);
	uint32_t getNbElem();
};

