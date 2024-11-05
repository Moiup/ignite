#pragma once

#include "DeviceBuffer.hpp"

template<IGEBufferUsage U>
DeviceBuffer<U>::DeviceBuffer(Device* device, VkDeviceSize size) :
	Buffer<U>(device, size)
{
	setMemoryPropertyFlags(
		IGEMemoryType::device
	);
	create();
}
