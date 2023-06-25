#pragma once

#include "vkbuffer.h"


/**
 * IBO将默认使用StageLoadRaw()的方式使用stage buffer填充数据，
 * 因此使用VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
*/
class VKIndexBuffer: public VKBuffer{
public:
    VKIndexBuffer(VKDevice* device, uint64_t size, uint32_t count): 
    VKBuffer(device, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    // VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    {
        m_indexCount = count;
    }

    uint32_t m_indexCount;
};