#pragma once
#include <vulkan/vulkan.h>

class ArrayBufferInfo
{
private:
	uint32_t _binding;
	VkShaderStageFlags _stage_flags;
	VkDescriptorType _descriptor_type;
	uint32_t _descriptor_count;

public:
	ArrayBufferInfo();

	void setBinding(uint32_t binding);
	void setStageFlags(VkShaderStageFlags stage_flags);
	void setDescriptorType(VkDescriptorType descriptor_type);
	void setDescriptorCount(uint32_t descriptor_count);

	const uint32_t getBinding() const;
	const VkShaderStageFlags getStageFlags() const;
	const VkDescriptorType getDescriptorType() const;
	const uint32_t getDescriptorCount() const;
};

