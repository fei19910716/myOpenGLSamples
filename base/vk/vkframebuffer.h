#pragma once

#include "base/utils.h"
#include "vkrenderpass.h"

#include <vector>
#include <cassert>

class VKFrameBuffer: public VKObject<VkFramebuffer>{

public:
    VKFrameBuffer(VKRenderPass* renderpass, const std::vector<VkImageView>& imageViews, VkExtent2D extent):
    m_renderpass(renderpass)
    {

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderpass->Handle();
        framebufferInfo.attachmentCount = (uint32_t)imageViews.size();
        framebufferInfo.pAttachments = imageViews.data();
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        assert(vkCreateFramebuffer(renderpass->DeviceHandle(), &framebufferInfo, nullptr, &handle) == VK_SUCCESS);
    }

    ~VKFrameBuffer(){
        vkDestroyFramebuffer(m_renderpass->DeviceHandle(),handle,nullptr);
    }

    VKRenderPass* m_renderpass = nullptr;
};