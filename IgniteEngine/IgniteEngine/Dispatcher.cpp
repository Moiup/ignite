#include "Dispatcher.h"

Dispatcher::Dispatcher() :
	_command_buffer{nullptr},
	_logical_device{DefaultConf::logical_device}
{
	;
}

Dispatcher::Dispatcher(CommandBuffer* command_buffer, LogicalDevice* logical_device) {
	_command_buffer = command_buffer;
	_logical_device = logical_device;
}

void Dispatcher::setCommandBuffer(CommandBuffer* command_buffer) {
	_command_buffer = command_buffer;
}

void Dispatcher::setLogicalDevice(LogicalDevice* logical_device) {
	_logical_device = logical_device;
}