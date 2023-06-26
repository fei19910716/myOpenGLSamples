#pragma once

#include "base/utils.h"
#include "vkbuffer.h"

#include <vector>
#include <string>
#include <cassert>

#include <stb/stb_image.h>

/**
 * Vulkan instance, stores all per-application states
*/
class VKImage : public VKObject<VkImage>{
public:
    ~VKImage(){
        vkDestroyImage(m_device->Handle(),handle,nullptr);
    }

    VKImage(VKDevice* device, const std::string& path): m_device(device)
    {
        
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        VKBuffer* stageBuffer = new VKBuffer(m_device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
												VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stageBuffer->MapLoadRaw(pixels);

        stbi_image_free(pixels);

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = texWidth;
        imageInfo.extent.height = texHeight;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vkCreateImage(device->Handle(), &imageInfo, nullptr, &handle);

        VkMemoryRequirements memReqs;
        vkGetImageMemoryRequirements(m_device->Handle(), handle, &memReqs);

        m_memory = new VKMemory(m_device,memReqs,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vkBindImageMemory(m_device->Handle(), handle, m_memory->Handle(), 0);

        TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            LoadBuffer(stageBuffer, {0,0,0}, {(uint32_t)texWidth,(uint32_t)texHeight,0});
        TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        SAFE_DELETE(stageBuffer);
    }

    VKImage(VKDevice* device, const VkImage& image): m_device(device)
    {
        this->handle = image;
    }

    void LoadRaw(const void* data) const
    {
        // todo
    }

    void LoadBuffer(VKBuffer* buffer, VkOffset3D imageOffset, VkExtent3D imageExtent) const
    {
        auto commandBuffer = VKCommandBufferPool::Instance(m_device)->AllocateCommandBuffers(1).front()->Handle();

        VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0; // If either of bufferRowLength & bufferImageHeight is zero, 
        region.bufferImageHeight = 0; // that aspect of the buffer memory is considered to be tightly packed according to the imageExtent.
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = imageOffset;
        region.imageExtent = imageExtent;

        vkCmdCopyBufferToImage(commandBuffer, buffer->Handle(), handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        vkEndCommandBuffer(commandBuffer);
		// Flushing the command buffer will also submit it to the queue and uses a fence to ensure that all commands have been executed before returning
		FlushCommandBuffer(commandBuffer);
    }


public:

    void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout) const
    {
        auto commandBuffer = VKCommandBufferPool::Instance(m_device)->AllocateCommandBuffers(1).front()->Handle();

        VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = handle;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        vkEndCommandBuffer(commandBuffer);
		// Flushing the command buffer will also submit it to the queue and uses a fence to ensure that all commands have been executed before returning
		FlushCommandBuffer(commandBuffer);
    }

    // End the command buffer and submit it to the queue
	// Uses a fence to ensure command buffer has finished executing before deleting it
	void FlushCommandBuffer(VkCommandBuffer commandBuffer) const
	{
		assert(commandBuffer != VK_NULL_HANDLE);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		// Submit to the queue
		vkQueueSubmit(m_device->m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_device->m_graphicsQueue);

		VKCommandBufferPool::Instance(m_device)->FreeCommandBuffer(commandBuffer);

        /**
         * Use fence to sync
       {
            VkFenceCreateInfo fenceCreateInfo = {};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = 0;
            VkFence fence;
            vkCreateFence(m_device->Handle(), &fenceCreateInfo, nullptr, &fence);

            vkQueueSubmit(m_device->m_graphicsQueue, 1, &submitInfo, fence);
            vkWaitForFences(m_device->Handle(), 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);

            vkDestroyFence(m_device->Handle(), fence, nullptr);
       }
       */
	}

    VKDevice* m_device = nullptr;
    VKMemory* m_memory = nullptr;
};