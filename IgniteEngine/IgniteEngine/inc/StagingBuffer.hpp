#include "Buffer.h"

template<IGEBufferUsage U>
class StagingBuffer : public Buffer<U>
{
public:
	StagingBuffer() = default;
	StagingBuffer(Device* device, VkDeviceSize size);
	StagingBuffer(Device* device, VkDeviceSize size, const void* values);

	void* map();
	void unmap();

	void setValues(const void* values);
	Pointer<uint8_t> getValues();
	void getValues(Pointer<uint8_t> data);
};
