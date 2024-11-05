#include "Buffer.h"

template<IGEBufferUsage U>
class DeviceBuffer : public Buffer<U>
{
public:
	DeviceBuffer() = default;
	DeviceBuffer(Device* device, VkDeviceSize size);
};
