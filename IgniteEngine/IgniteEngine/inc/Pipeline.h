#pragma once

#include "Pipeline.hpp"

template<IGEBufferUsage T>
void Pipeline::setStorageBuffer(
	const std::string& name,
	const Buffer<T>& buff
) {
	VkWriteDescriptorSet& write = setWriteDescriptorSet(name);
	_descriptor_buffer_infos[name].resize(1);
	VkDescriptorBufferInfo& desc_buf_info = _descriptor_buffer_infos[name][0];
	desc_buf_info.buffer = buff.getBuffer();
	desc_buf_info.offset = 0;
	desc_buf_info.range = buff.getCapacity();
	write.pBufferInfo = _descriptor_buffer_infos[name].data();
	write.pImageInfo = nullptr;
	write.pTexelBufferView = nullptr;
}