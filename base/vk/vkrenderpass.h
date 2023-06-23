#pragma once

#include "vkdevice.h"

#include <cassert>
#include <vector>

using namespace std;

class VKRenderPass: public VKObject<VkRenderPass>{
public:
    VKRenderPass(
        VKDevice* device,  
        vector<VkAttachmentDescription> attachments, 
        vector<VkSubpassDescription> supass,
        vector<VkSubpassDependency> dependency):
    m_device(device)
    {

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = (uint32_t)attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = (uint32_t)supass.size();
        renderPassInfo.pSubpasses = supass.data();
        renderPassInfo.dependencyCount = (uint32_t)dependency.size();
        renderPassInfo.pDependencies = dependency.data();

        assert(vkCreateRenderPass(device->Handle(), &renderPassInfo, nullptr, &handle) == VK_SUCCESS);
    }

    ~VKRenderPass(){
        vkDestroyRenderPass(m_device->Handle(),handle,nullptr);
    }


    VkDevice DeviceHandle() const{
        return m_device->Handle();
    }

private:
    VKDevice* m_device = nullptr;
};