#pragma once

#include "vkbuffer.h"

class VKUniformBuffer: VKBuffer{
public:
    VKUniformBuffer(VKDevice* device, uint64_t size): 
    VKBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,size)
    {

    }
};


