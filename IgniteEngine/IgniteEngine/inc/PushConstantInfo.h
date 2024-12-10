#pragma once

#include <vulkan/vulkan.h>

class PushConstantInfo
{
private:
	VkPushConstantRange _constant_range{};


public:
	PushConstantInfo();

	void setPushConstantRange(VkPushConstantRange& constant_range);
	void setStageFlags(VkShaderStageFlags stage_flags);
	void setOffset(uint32_t offset);
	void setSize(uint32_t size);

	const VkPushConstantRange& getPushConstantRange() const;
	const VkShaderStageFlags getStageFlags() const;
	const uint32_t getOffset() const;
	const uint32_t getSize() const;
};

