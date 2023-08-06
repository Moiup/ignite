#include "Dispatcher.h"

Dispatcher::Dispatcher() :
	_logical_device{ DefaultConf::logical_device },
	_command_pool{ DefaultConf::command_pool },
	_command_buffer {}
{
	;
}

Dispatcher::Dispatcher(
	LogicalDevice* logical_device,
	CommandPool* command_pool
) :
	_logical_device{logical_device},
	_command_pool{command_pool},
	_command_buffer{}
{
	;
}

void Dispatcher::setLogicalDevice(LogicalDevice* logical_device) {
	_logical_device = logical_device;
}

void Dispatcher::setCommandPool(CommandPool* command_pool) {
	_command_pool = command_pool;
}

void Dispatcher::create() {
	_command_buffer = _command_pool->createCommandBuffer();
}

void Dispatcher::destroy() {
	_command_buffer.free();
}