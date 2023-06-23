#pragma once

#include "vkbuffer.h"

/**
 * UBO将默认使用MapLoadRaw()的方式填充数据，
 * 因此使用VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
*/
class VKUniformBuffer: VKBuffer{
public:
    VKUniformBuffer(VKDevice* device, uint64_t size): 
    VKBuffer(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    {

    }
};


