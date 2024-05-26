#pragma once

#include "Ressource.h"
#include "Pointer.h"
#include "Image.h"
#include <unordered_map>

class Buffer : public Ressource
{
private:
	VkBuffer _buffer{ nullptr };
	VkBufferCreateInfo _buffer_info{};
	VkDeviceSize _capacity{ 0 };
	VkDeviceSize _size{ 0 };

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
	void getValues(Pointer<uint8_t> data);

	uint32_t getSize();
	uint32_t getCapacity();
	
	void* map();
	void unmap();

	// Copy image to buffer
	void copy(Image& img);

protected:
	void createBuffer();
	void destroyBuffer();
	void getMemoryRequirements();
};

