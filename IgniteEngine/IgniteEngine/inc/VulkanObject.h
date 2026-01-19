#ifndef _VULKAN_OBJECT_H_
#define _VULKAN_OBJECT_H_


template <typename T>
class VulkanObject {

public:
    virtual const T& vkObj() const = 0;
};


#endif