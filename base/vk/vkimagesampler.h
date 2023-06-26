#pragma once

#include "base/utils.h"
#include "vkimage.h"
#include "vkdevice.h"

#include <vector>
#include <string>
#include <cassert>


/**
 * Vulkan instance, stores all per-application states
*/
class VKImageSampler : public VKObject<VkSampler>{
public:
    VKImageSampler(VKDevice* device, const VkSamplerCreateInfo& samplerInfo): m_device(device)
    {
        if (vkCreateSampler(m_device->Handle(), &samplerInfo, nullptr, &handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    ~VKImageSampler(){
        vkDestroySampler(m_device->Handle(),handle,nullptr);
    }

    VKDevice* m_device = nullptr;
};