#pragma once

#include "base/utils.h"
#include "vkimage.h"
#include "vkdevice.h"
#include "vkcommandbuffer.h"
#include "vkcommandbufferpool.h"
#include "vkmemory.h"

#include <vector>
#include <string>
#include <cassert>

#define DEFAULT_FENCE_TIMEOUT 100000000000

class VKBuffer: public VKObject<VkBuffer>{
public:

    VKBuffer(VKDevice* device, uint64_t size, VkFlags usage, VkFlags memoryFlags):
	m_usageFlags(usage),
	m_device(device),
	m_size(size)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = m_usageFlags;

		// Create a new buffer
		vkCreateBuffer(device->Handle(), &bufferInfo, nullptr, &handle);

		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(device->Handle(), handle, &memReqs);
		m_memory = new VKMemory(m_device,memReqs,memoryFlags);

		// Bind memory to buffer
		vkBindBufferMemory(device->Handle(), handle, m_memory->Handle(), 0);
	}

	~VKBuffer(){
		SAFE_DELETE(m_memory);
		vkDestroyBuffer(m_device->Handle(), handle, nullptr);
	}
	
    void StageLoadRaw(const void* data)
	{
        // Static data like vertex and index buffer should be stored on the device memory
		// for optimal (and fastest) access by the GPU
		//
		// To achieve this we use so-called "staging buffers" :
		// - Create a buffer that's visible to the host (and can be mapped)
		// - Copy the data to this buffer
		// - Create another buffer that's local on the device (VRAM) with the same size
		// - Copy the data from the host to the device using a command buffer
		// - Delete the host visible (staging) buffer
		// - Use the device local buffers for rendering

		auto stageBuffer = new VKBuffer(m_device,m_size,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stageBuffer->MapLoadRaw(data);

		assert((m_usageFlags  & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)  == 0);
		assert((~m_usageFlags  & VK_BUFFER_USAGE_TRANSFER_DST_BIT)  == 0);
		assert((~m_memory->m_flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == 0);


		// Buffer copies have to be submitted to a queue, so we need a command buffer for them
		// Note: Some devices offer a dedicated transfer queue (with only the transfer bit set) that may be faster when doing lots of copies
		
		auto commandBuffer = VKCommandBufferPool::Instance(m_device)->AllocateCommandBuffers(1).front()->Handle();

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
			{
				// Put buffer region copies into command buffer
				VkBufferCopy copyRegion = {};
				copyRegion.size = m_size;
				vkCmdCopyBuffer(commandBuffer, stageBuffer->Handle(), handle, 1, &copyRegion);
			}
		vkEndCommandBuffer(commandBuffer);
		// Flushing the command buffer will also submit it to the queue and uses a fence to ensure that all commands have been executed before returning
		FlushCommandBuffer(commandBuffer);
    }

	void MapLoadRaw(const void* bufferData){
		// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		assert((~m_memory->m_flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0);

		void *data;
		vkMapMemory(m_device->Handle(), m_memory->Handle(), 0, m_memory->m_size, 0, &data);
		memcpy(data, bufferData, m_size);
		vkUnmapMemory(m_device->Handle(), m_memory->Handle());
	}

	// End the command buffer and submit it to the queue
	// Uses a fence to ensure command buffer has finished executing before deleting it
	void FlushCommandBuffer(VkCommandBuffer commandBuffer)
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
    VkFlags m_usageFlags;
	VkDeviceSize m_size;
};