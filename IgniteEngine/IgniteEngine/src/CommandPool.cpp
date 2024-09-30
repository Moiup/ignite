#include "CommandPool.h"

CommandPool::CommandPool() :
	_pool{},
	_flags{},
	_family_index{},
	_created{false}
{
	;
}

void CommandPool::setDevice(Device* device) {
	_device = device;
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
		_device->getDevice(),
		&pool_info,
		nullptr,
		&_pool
	);

	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating command pool!");
	}

	_created = true;
}

void CommandPool::reset() {
	VkResult vk_result = vkResetCommandPool(
		_device->getDevice(),
		_pool,
		VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
	);
}

void CommandPool::destroy() {
	if (!_created) {
		return;
	}

	vkDestroyCommandPool(
		_device->getDevice(),
		_pool,
		nullptr
	);
}

const VkCommandPool& CommandPool::getPool() const {
	return _pool;
}