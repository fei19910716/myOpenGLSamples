#pragma once

#include "vulkan/vulkan.h"
#include "base/utils.h"
#include "vkrenderpass.h"
#include "vkswapchain.h"

#include <vector>
#include <cassert>

class VKFrameBuffer: public VKObject<VkFramebuffer>{

public:
    VKFrameBuffer(VKSwapChain* swapchain, VKRenderPass* renderpass){

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderpass->Handle();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = swapchain->GetImageViews().data();
        framebufferInfo.width = swapchain->GetExtent2D().width;
        framebufferInfo.height = swapchain->GetExtent2D().height;
        framebufferInfo.layers = 1;

        assert(vkCreateFramebuffer(swapchain->GetDevice()->Handle(), &framebufferInfo, nullptr, &handle) == VK_SUCCESS);
    }

private:
    VkPipelineLayout m_pipelineLayout;
};