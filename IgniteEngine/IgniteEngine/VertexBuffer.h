#pragma once
#include "Buffer.h"

#include <vector>

class VertexBuffer : public Buffer
{
private:
	uint32_t _first_binding;

public:
	VertexBuffer();

	void create();

	void setFirstBinding(uint32_t binding_desc);

	uint32_t getFirstBinding();
};

