#include "Buffer.h"

template<IGEBufferUsage U>
class StagingBuffer : public Buffer<U>
{
private:
	void* _map_ptr { nullptr };

public:
	StagingBuffer() = default;
	StagingBuffer(Device* device, VkDeviceSize size);
	template<class T>
	StagingBuffer(Device* device, VkDeviceSize size, const std::vector<T>& v);
	StagingBuffer(Device* device, VkDeviceSize size, const void* values, const int32_t values_size);

	void* map();
	void unmap();

	template <class T>
	void setValues(const std::vector<T>& v);
	void setValues(const void* values, const int32_t size);
	Pointer<uint8_t> getValues();
	void getValues(Pointer<uint8_t> data);
};
