#include "Fence.h"

Fence::Fence() {
    _shared_count = new int32_t(1);
}

Fence::Fence(Device& device, VkFenceCreateFlags flags) :
    Fence::Fence()
{
    _device = &device;
    
    VkFenceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = flags,
    };
    create(info);
}

Fence::Fence(const Fence& f){
    *this = f;
}

Fence::Fence(Fence&& f){
    *this = std::move(f);
}

Fence::~Fence(){
    destroy();
}

Fence& Fence::operator=(const Fence& f){
    destroy();
    _shared_count = f._shared_count;
    *_shared_count += 1;

    _device = f._device;
    _fence = f._fence;

    return *this;
}

Fence& Fence::operator=(Fence&& f){
    destroy();
    _shared_count = std::move(f)._shared_count;
    _device = std::move(f)._device;
    _fence = std::move(f)._fence;

    f._fence = nullptr;
    f._device = nullptr;
    f._shared_count = nullptr;

    return *this;
}

const VkFence& Fence::vkObj() const {
    return _fence;
}

VkResult Fence::getStatus(){
    return vkGetFenceStatus(
        _device->vkObj(),
        _fence
    );
}

void Fence::create(VkFenceCreateInfo& info){
    vkCreateFence(
        _device->vkObj(),
        &info,
        nullptr,
        &_fence
    );
}

void Fence::destroy(){
    if(!_shared_count){
        return;
    }

    *_shared_count -= 1;
    if(*_shared_count){
        return;
    }
    delete _shared_count;
    _shared_count = nullptr;

    if(!_fence){
        return;
    }
    vkDestroyFence(
        _device->vkObj(),
        _fence,
        nullptr
    );
}