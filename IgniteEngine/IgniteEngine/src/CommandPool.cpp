#include "CommandPool.h"

CommandPool::CommandPool() :
	_pool{nullptr},
	_device{nullptr},
	_flags{},
	_family_index{},
	_shared_count{nullptr}
{
	_command_buffers = new std::vector<VkCommandBuffer>;
	_shared_count = new int(1);
}

CommandPool::CommandPool(const CommandPool& cp)
{
	*this = cp;
}

CommandPool& CommandPool::operator=(const CommandPool& cp) {
	_pool = { cp._pool };
	_device = { cp._device };
	_flags = { cp._flags };
	_family_index = { cp._family_index };
	_command_buffers = { cp._command_buffers };
	_shared_count = { cp._shared_count };
	*_shared_count += 1;

	return *this;
}

bool CommandPool::operator==(const CommandPool& cp) {
	return _pool == cp._pool;
}

CommandPool::~CommandPool() {
	*_shared_count -= 1;
	if (!*_shared_count) {
		destroy();
		delete _shared_count;
	}
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
}

void CommandPool::reset() {
	if (_command_buffers->empty()) {
		return;
	}

	resetCommandBuffers();
	VkResult vk_result = vkResetCommandPool(
		_device->getDevice(),
		_pool,
		VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
	);
}

const VkCommandPool& CommandPool::getPool() const {
	return _pool;
}

CommandBuffer& CommandPool::newCommandBuffer() {
	VkCommandBuffer cmd_buf{ nullptr };
	VkCommandBufferAllocateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.commandPool = _pool;
	info.commandBufferCount = 1;
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	VkResult vk_result = vkAllocateCommandBuffers(
		_device->getDevice(),
		&info,
		&cmd_buf
	);

	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating command buffer!");
	}

	CommandBuffer command_buffer;
	command_buffer._command_buffer = cmd_buf;
	_command_buffers->push_back(cmd_buf);

	return command_buffer;
}

void CommandPool::destroy() {
	if (!_pool) {
		return;
	}

	reset();
	
	delete _command_buffers;

	vkDestroyCommandPool(
		_device->getDevice(),
		_pool,
		nullptr
	);
}

void CommandPool::resetCommandBuffers() {
	vkFreeCommandBuffers(
		_device->getDevice(),
		_pool,
		_command_buffers->size(),
		_command_buffers->data()
	);
	_command_buffers->clear();
}
