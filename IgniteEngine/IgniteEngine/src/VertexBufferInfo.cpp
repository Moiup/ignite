#include "VertexBufferInfo.h"

VertexBufferInfo::VertexBufferInfo() :
	_stride{},
	_location{},
	_format{},
	_first_binding{}
{
	;
}

void VertexBufferInfo::setStride(uint32_t stride) {
	_stride = stride;
}

void VertexBufferInfo::setLocation(uint32_t location) {
	_location = location;
}

void VertexBufferInfo::setFormat(VkFormat format) {
	_format = format;
}

void VertexBufferInfo::setFirstBinding(uint32_t first_binding) {
	_first_binding = first_binding;
}

const uint32_t VertexBufferInfo::getStride() const {
	return _stride;
}

const uint32_t VertexBufferInfo::getLocation() const {
	return _location;
}

const VkFormat VertexBufferInfo::getFormat() const {
	return _format;
}

const uint32_t VertexBufferInfo::getFirstBinding() const {
	return _first_binding;
}
