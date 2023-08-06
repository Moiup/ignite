#pragma once

#include "CommandBuffer.h"
#include "DefaultConf.h"

class Dispatcher
{
private:
	CommandBuffer* _command_buffer;
	LogicalDevice* _logical_device;

public:
	Dispatcher();
	Dispatcher(CommandBuffer* command_buffer, LogicalDevice* logical_device);

	void setCommandBuffer(CommandBuffer* command_buffer);
	void setLogicalDevice(LogicalDevice* logical_device);

	virtual void dispatch() = 0;
};

