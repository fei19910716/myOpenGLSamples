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
class VKImageView : public VKObject<VkImageView>{
public:
    VKImageView(VKDevice* device, VKImage* image, VkFormat format): m_device(device)
    {
        VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = image->Handle();
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            assert(vkCreateImageView(device->Handle(), &createInfo, nullptr, &handle) == VK_SUCCESS);
    }

    ~VKImageView(){
        vkDestroyImageView(m_device->Handle(),handle,nullptr);
    }

    VKDevice* m_device = nullptr;
};