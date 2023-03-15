#pragma once

#include <vulkan/vulkan.h>
#include <iostream>

class VertexBufferInfo
{
private:
	uint32_t _stride;
	uint32_t _location;
	VkFormat _format;
	uint32_t _first_binding;

public:
	VertexBufferInfo();

	void setStride(uint32_t stride);
	void setLocation(uint32_t location);
	void setFormat(VkFormat format);
	void setFirstBinding(uint32_t first_binding);

	const uint32_t getStride() const;
	const uint32_t getLocation() const;
	const VkFormat getFormat() const;
	const uint32_t getFirstBinding() const;
};

