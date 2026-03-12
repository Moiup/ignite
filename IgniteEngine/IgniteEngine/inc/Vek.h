#ifndef _VEK_H_
#define _VEK_H_

#include <iostream>
#include <vector>
#include <type_traits>
#include "CommandBuffer.h"

template<typename T, typename VK>
class Vek_impl {
private:
    std::vector<T*> _objs{};
    std::vector<VK> _vk_objs{};

public:
    Vek_impl();

    std::vector<T*>& objs();
    std::vector<VK>& vkObjs();
    
    uint32_t size();
    uint32_t capacity();
    void push_back(T& obj);
    void clear();
};

template<typename T, typename VK>
Vek_impl<T, VK>::Vek_impl(){
    static_assert(std::is_base_of<VulkanObject<VK>, T>(), "T type should be a Vulkan object!");
}

template<typename T, typename VK>
std::vector<T*>& Vek_impl<T, VK>::objs() {
    return _objs;
}

template<typename T, typename VK>
std::vector<VK>& Vek_impl<T, VK>::vkObjs() {
    return _vk_objs;
}

template<typename T, typename VK>
uint32_t Vek_impl<T, VK>::size(){
    return _objs.size();
}

template<typename T, typename VK>
uint32_t Vek_impl<T, VK>::capacity(){
    return _objs.capacity();
}

template<typename T, typename VK>
void Vek_impl<T, VK>::push_back(T& obj){
    _vk_objs.push_back(obj.vkObj());
    _objs.push_back(&obj);
}

template<typename T, typename VK>
void Vek_impl<T, VK>::clear(){
    _vk_objs.clear();
    _objs.clear();
}

template<typename T, typename VK>
class Vek_tmp : public Vek_impl<T, VK>{
public:
    Vek_tmp();
};

template<typename T, typename VK>
Vek_tmp<T, VK>::Vek_tmp() : Vek_impl<T, VK>() 
{
    static_assert(!std::is_same<VK, void*>(), "Error! Not a valid Vulkan buffer object!");
}

template<typename T>
using Vek = Vek_tmp<T,
        std::conditional_t<std::is_same<T, CommandBuffer>::value, VkCommandBuffer, void*>
        >;

#endif