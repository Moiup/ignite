#include "Ressource.h"
#include "Pointer.h"
#include <unordered_map>

template <IGEBufferUsage U>
class Buffer : public Ressource
{
protected:
	VkBuffer _buffer{ nullptr };
	VkBufferCreateInfo _buffer_info{};
	VkDeviceSize _capacity{ 0 };
	VkDeviceSize _size{ 0 };

	Buffer();
	Buffer(
		Device* device,
		VkDeviceSize size
	);

public:
	Buffer(const Buffer<U>& buf);
	Buffer(Buffer<U>&& buf);
	~Buffer();

	Buffer<U>& operator=(const Buffer<U>& buf);
	Buffer<U>& operator=(Buffer<U>&& buf);

	void setValues(void* values);
	void bind();
	void destroy();

	const VkBuffer& getBuffer() const;
	//const VkBuffer& getBuffer() const;
	Pointer<uint8_t> getValues();
	void setValues(Pointer<uint8_t> data);

	const uint32_t& getSize() const;
	const uint32_t& getCapacity() const;

	void* map();
	void unmap();

protected:
	void create();

	void setPNext(const void* p_next);
	void setFlags(VkBufferCreateFlags flags);
	void setSize(VkDeviceSize size);
	void setCapacity(VkDeviceSize capacity);
	void setUsage(VkBufferUsageFlags usage);
	void setSharingMode(VkSharingMode sharing_mode);
	void setQueueFamilyIndexCount(uint32_t family_index_count);
	void setPQueueFamilyIndices(const uint32_t* p_queue_family_indices);

	void createBuffer();
	void destroyBuffer();
	void getMemoryRequirements();
};
