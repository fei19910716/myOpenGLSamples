#pragma once

#include "base/utils.h"
#include "vkdevice.h"
#include "vkcommandbuffer.h"

#include <cassert>
#include <vector>
#include <string>


class VKCommandBufferPool: public VKObject<VkCommandPool>{
public:

    static VKCommandBufferPool* Instance(VKDevice* device){
        static VKCommandBufferPool pool(device);
        return &pool;
    }

    ~VKCommandBufferPool(){
        vkDestroyCommandPool(m_device->Handle(),handle,nullptr);
    }

    void FreeCommandBuffer(VkCommandBuffer buffer){
        vkFreeCommandBuffers(m_device->Handle(), handle, 1, &buffer);
    }

    std::vector<VKCommandBuffer*> AllocateCommandBuffers(uint32_t count)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = handle;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = count;

        std::vector<VkCommandBuffer> buffers(count);
        if (vkAllocateCommandBuffers(m_device->Handle(), &allocInfo, buffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        std::vector<VKCommandBuffer*> result;
        for(auto buffer: buffers){
            result.push_back(new VKCommandBuffer(buffer));
        }

        return result;
    }
private:
    VKCommandBufferPool(VKDevice* device): m_device(device)
    {
        
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = device->PhysicalDevice()->GraphicsQueueFamily();

        if (vkCreateCommandPool(m_device->Handle(), &poolInfo, nullptr, &handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    VKDevice* m_device = nullptr;
};