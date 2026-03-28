#ifndef _FENCE_H_
#define _FENCE_H_

#include "Device.h"
#include "VulkanObject.h"

class Fence : public VulkanObject<VkFence> {
private:
    int32_t* _shared_count {nullptr};

    Device* _device{nullptr};
    VkFence _fence{nullptr};

public:
    Fence();
    Fence(Device& device, VkFenceCreateFlags flags = 0);
    Fence(const Fence& f);
    Fence(Fence&& f);
    ~Fence();

    Fence& operator=(const Fence& f);
    Fence& operator=(Fence&& f);

    const VkFence& vkObj() const;
    VkResult getStatus();

private:
    void create(VkFenceCreateInfo& info);
    void destroy();
};

#endif