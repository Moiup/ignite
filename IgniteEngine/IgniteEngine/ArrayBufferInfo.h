#pragma once
#include <vulkan/vulkan.h>

class ArrayBufferInfo
{
private:
	uint32_t _binding;
	VkShaderStageFlags _stage_flags;
	VkDescriptorType _descriptor_type;

public:
	ArrayBufferInfo();

	void setBinding(uint32_t binding);
	void setStageFlags(VkShaderStageFlags stage_flags);
	void setDescriptorType(VkDescriptorType descriptor_type);

	const uint32_t getBinding() const;
	const VkShaderStageFlags getStageFlags() const;
	const VkDescriptorType getDescriptorType() const;
};

