#pragma once
#include "Ressource.h"
#include "Pointer.h"
#include <unordered_map>

class Buffer : public Ressource
{
private:
	VkBuffer _buffer;
	VkBufferCreateInfo _buffer_info;
	VkDeviceSize _capacity;
	VkDeviceSize _size;

public:
	Buffer();

	void create();
	void setValues(void* values);
	void bind();
	void destroy();

	void setPNext(const void * p_next);
	void setFlags(VkBufferCreateFlags flags);
	void setSize(VkDeviceSize size);
	void setCapacity(VkDeviceSize capacity);
	void setUsage(VkBufferUsageFlags usage);
	void setSharingMode(VkSharingMode sharing_mode);
	void setQueueFamilyIndexCount(uint32_t family_index_count);
	void setPQueueFamilyIndices(const uint32_t* p_queue_family_indices);

	VkBuffer& getBuffer();
	Pointer<uint8_t> getValues();


protected:
	void createBuffer();
	void destroyBuffer();
	void getMemoryRequirements();
};

