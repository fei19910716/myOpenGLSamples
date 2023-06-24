#pragma once

#include "vkbuffer.h"

/**
 * VBO将默认使用StageLoadRaw()的方式使用stage buffer填充数据，
 * 因此使用VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
*/
class VKVertexBuffer: public VKBuffer{
public:
    VKVertexBuffer(VKDevice* device, uint64_t size, uint32_t count): 
    VKBuffer(device, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
    // VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    {
        m_vertexCount = count;
    }

    uint32_t m_vertexCount;
};
