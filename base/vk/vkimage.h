#pragma once

#include "base/utils.h"
#include "vkdevice.h"

#include <vector>
#include <string>
#include <cassert>


/**
 * Vulkan instance, stores all per-application states
*/
class VKImage : public VKObject<VkImage>{
public:
    ~VKImage(){
        // vkDestroyImage(m_device->Handle(),handle,nullptr);
    }

    VKImage(VKDevice* device, const VkImage& image): m_device(device)
    {
        this->handle = image;
    }

    VKDevice* m_device = nullptr;
};