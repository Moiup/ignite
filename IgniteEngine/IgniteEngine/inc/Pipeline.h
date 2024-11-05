#pragma once

#include "Pipeline.hpp"

template<IGEBufferUsage U>
void Pipeline::setWriteDescriptorSet(
	std::unordered_map<std::string, Buffer<U>*>& buffer_arr,
	std::unordered_map<std::string, ArrayBufferInfo>& buffer_info_arr,
	std::vector<VkWriteDescriptorSet>& write_descriptor_set_arr
) {
	for (std::pair<std::string, Buffer<U>*> buff_data : buffer_arr) {
		std::string name = buff_data.first;
		Buffer<U>* buff = buff_data.second;
		ArrayBufferInfo& info = buffer_info_arr[name];
		VkDescriptorBufferInfo* descriptor_buffer_info = new VkDescriptorBufferInfo();

		descriptor_buffer_info->buffer = buff->getBuffer();
		descriptor_buffer_info->offset = 0;
		descriptor_buffer_info->range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet writes{};
		writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes.pNext = nullptr;
		writes.dstSet = _descriptor_sets[0];
		writes.dstBinding = info.getBinding();
		writes.dstArrayElement = 0;
		writes.descriptorCount = 1;
		writes.descriptorType = info.getDescriptorType();
		writes.pImageInfo = nullptr;
		writes.pBufferInfo = descriptor_buffer_info;
		writes.pTexelBufferView = nullptr;

		write_descriptor_set_arr.push_back(writes);
	}
}