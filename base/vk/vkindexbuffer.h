#pragma once

#include "vkbuffer.h"

class VKIndecBuffer: VKBuffer{
public:
    VKIndecBuffer(VKDevice* device, uint64_t size): 
    VKBuffer(device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,size)
    {

    }
};