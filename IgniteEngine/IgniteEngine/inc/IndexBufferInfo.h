#pragma once
#include <iostream>
#include <vulkan/vulkan.h>

class IndexBufferInfo
{
private:
	uint32_t _nb_elem;
	VkIndexType _index_type;

public:
	IndexBufferInfo();

	void setNbElem(uint32_t nb_elem);
	void setIndexType(VkIndexType index_type);

	const uint32_t getNbElem();
	const VkIndexType getIndexType() const;
};

