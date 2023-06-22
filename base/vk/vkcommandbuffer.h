#pragma once

#include "vulkan/vulkan.h"
#include "base/utils.h"
#include "vkdevice.h"

#include <cassert>
#include <vector>
#include <string>


class VKCommandBuffer: public VKObject<VkCommandBuffer>{
public:
    VKCommandBuffer(VKDevice* device){

        VkCommandPool commandPool;
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = device->PhysicalDevice()->GraphicsQueueFamily();

        if (vkCreateCommandPool(device->Handle(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(device->Handle(), &allocInfo, &handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

};