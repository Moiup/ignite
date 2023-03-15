#include "CommandPool.h"

CommandPool::CommandPool() :
	_pool{},
	_logical_device{ nullptr },
	_flags{},
	_family_index{},
	_created{false}
{
	;
}

void CommandPool::setLogicalDevice(VkDevice* logical_device) {
	_logical_device = logical_device;
}

void CommandPool::setFlags(VkCommandPoolCreateFlagBits flags) {
	_flags = flags;
}

void CommandPool::setQueueFamilyIndex(uint32_t family_index) {
	_family_index = family_index;
}

void CommandPool::create() {
	VkCommandPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.pNext = nullptr;
	pool_info.flags = _flags;
	pool_info.queueFamilyIndex = _family_index;

	VkResult vk_result = vkCreateCommandPool(
		*_logical_device,
		&pool_info,
		nullptr,
		&_pool
	);

	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating command pool!");
	}

	_created = true;
}

void CommandPool::destroy() {
	if (!_created) {
		return;
	}

	vkDestroyCommandPool(*_logical_device, _pool, nullptr);
}

CommandBuffer CommandPool::createCommandBuffer(VkCommandBufferLevel level) {
	CommandBuffer cmd_buffer{};
	cmd_buffer.setLogicalDevice(_logical_device);
	cmd_buffer.setCommandPool(&_pool);
	cmd_buffer.setLevel(level);

	cmd_buffer.create();

	return cmd_buffer;
}