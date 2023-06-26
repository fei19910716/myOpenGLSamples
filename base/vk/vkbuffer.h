#pragma once

#include "base/utils.h"
#include "vkimage.h"
#include "vkdevice.h"
#include "vkcommandbuffer.h"
#include "vkcommandbufferpool.h"

#include <vector>
#include <string>
#include <cassert>

#define DEFAULT_FENCE_TIMEOUT 100000000000

class VKBuffer: public VKObject<VkBuffer>{
public:

    VKBuffer(VKDevice* device, uint64_t size, VkFlags usage, VkFlags memoryFlags):
	m_usageFlags(usage),
	m_memoryFlags(memoryFlags),
	m_device(device)
	{
		// Prepare and initialize a uniform buffer block containing shader uniforms
		// Single uniforms like in OpenGL are no longer present in Vulkan. All Shader uniforms are passed via uniform buffer blocks
		VkMemoryRequirements memReqs;

		// Vertex shader uniform buffer block
		VkBufferCreateInfo bufferInfo = {};
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = m_requestSize = size;
		// This buffer will be used as a uniform buffer
		bufferInfo.usage = m_usageFlags;

		// Create a new buffer
		(vkCreateBuffer(device->Handle(), &bufferInfo, nullptr, &handle));
		// Get memory requirements including size, alignment and memory type
		vkGetBufferMemoryRequirements(device->Handle(), handle, &memReqs);
		allocInfo.allocationSize = m_allocationSize = memReqs.size;
		// Get the memory type index that supports host visible memory access
		// Most implementations offer multiple memory types and selecting the correct one to allocate memory from is crucial
		// We also want the buffer to be host coherent so we don't have to flush (or sync after every update.
		// Note: This may affect performance so you might not want to do this in a real world application that updates buffers on a regular base
		allocInfo.memoryTypeIndex = getMemoryTypeIndex(memReqs.memoryTypeBits, m_memoryFlags);
		
        // Allocate memory for the uniform buffer
		(vkAllocateMemory(device->Handle(), &allocInfo, nullptr, &m_memory));
		// Bind memory to buffer
		(vkBindBufferMemory(device->Handle(), handle, m_memory, 0));
	}

	~VKBuffer(){
		vkDestroyBuffer(m_device->Handle(), handle, nullptr);
        vkFreeMemory(m_device->Handle(), m_memory, nullptr);
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

		VKBuffer* stageBuffer = new VKBuffer(m_device, m_requestSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
												VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stageBuffer->MapLoadRaw(data);

		assert((m_usageFlags  & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)  == 0);
		assert((~m_usageFlags  & VK_BUFFER_USAGE_TRANSFER_DST_BIT)  == 0);
		assert((~m_memoryFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == 0);


		// Buffer copies have to be submitted to a queue, so we need a command buffer for them
		// Note: Some devices offer a dedicated transfer queue (with only the transfer bit set) that may be faster when doing lots of copies
		
		auto commandBuffer = VKCommandBufferPool::Instance(m_device)->AllocateCommandBuffers(1).front()->Handle();

		// Put buffer region copies into command buffer
		VkBufferCopy copyRegion = {};
		copyRegion.size = m_requestSize;

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
			{
				vkCmdCopyBuffer(commandBuffer, stageBuffer->Handle(), handle, 1, &copyRegion);
			}
		vkEndCommandBuffer(commandBuffer);
		// Flushing the command buffer will also submit it to the queue and uses a fence to ensure that all commands have been executed before returning
		FlushCommandBuffer(commandBuffer);
    }

	void MapLoadRaw(const void* bufferData){
		// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		assert((~m_memoryFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0);

		void *data;
		vkMapMemory(m_device->Handle(), m_memory, 0, m_allocationSize, 0, &data);
		memcpy(data, bufferData, m_requestSize);
		vkUnmapMemory(m_device->Handle(), m_memory);
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

		// Create fence to ensure that the command buffer has finished executing
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = 0;
		VkFence fence;
		vkCreateFence(m_device->Handle(), &fenceCreateInfo, nullptr, &fence);

		// Submit to the queue
		vkQueueSubmit(m_device->m_graphicsQueue, 1, &submitInfo, fence);
		// Wait for the fence to signal that command buffer has finished executing
		vkWaitForFences(m_device->Handle(), 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);

		auto pool = VKCommandBufferPool::Instance(m_device);
		vkDestroyFence(m_device->Handle(), fence, nullptr);
		vkFreeCommandBuffers(m_device->Handle(), pool->Handle(), 1, &commandBuffer);
	}

    // This function is used to request a device memory type that supports all the property flags we request (e.g. device local, host visible)
	// Upon success it will return the index of the memory type that fits our requested memory properties
	// This is necessary as implementations can offer an arbitrary number of memory types with different
	// memory properties.
	// You can check http://vulkan.gpuinfo.org/ for details on different memory configurations
	uint32_t getMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties)
	{
        auto deviceMemoryProperties = m_device->PhysicalDevice()->deviceMemoryProperties;
		// Iterate over all memory types available for the device used in this example
		for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
		{
			if ((typeBits & 1) == 1)
			{
				if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					return i;
				}
			}
			typeBits >>= 1;
		}

		throw "Could not find a suitable memory type!";
	}

    VKDevice* m_device = nullptr;
    VkDeviceMemory m_memory;
    VkFlags m_usageFlags;
	VkFlags m_memoryFlags;
	VkDeviceSize m_requestSize;
	VkDeviceSize m_allocationSize;
};