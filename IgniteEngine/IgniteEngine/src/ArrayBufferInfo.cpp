#include "ArrayBufferInfo.h"

ArrayBufferInfo::ArrayBufferInfo() :
	_binding{},
	_stage_flags{},
	_descriptor_type{},
	_descriptor_count{1}
{
	;
}

void ArrayBufferInfo::setBinding(uint32_t binding) {
	_binding = binding;
}

void ArrayBufferInfo::setStageFlags(VkShaderStageFlags stage_flags) {
	_stage_flags = stage_flags;
}

void ArrayBufferInfo::setDescriptorType(VkDescriptorType descriptor_type) {
	_descriptor_type = descriptor_type;
}

void ArrayBufferInfo::setDescriptorCount(uint32_t descriptor_count) {
	_descriptor_count = descriptor_count;
}

const uint32_t ArrayBufferInfo::getBinding() const {
	return _binding;
}

const VkShaderStageFlags ArrayBufferInfo::getStageFlags() const {
	return _stage_flags;
}

const VkDescriptorType ArrayBufferInfo::getDescriptorType() const {
	return _descriptor_type;
}

const uint32_t ArrayBufferInfo::getDescriptorCount() const {
	return _descriptor_count;
}