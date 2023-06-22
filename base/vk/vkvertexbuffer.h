#pragma once

#include "vkbuffer.h"

class VKVertexBuffer: VKBuffer{
public:
    VKVertexBuffer(VKDevice* device, uint64_t size): 
    VKBuffer(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,size)
    {

    }
};
