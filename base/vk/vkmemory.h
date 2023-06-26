#pragma once

#include "base/utils.h"
#include "vkdevice.h"

class VKMemory: public VKObject<VkDeviceMemory>{
public:
    VKMemory(VKDevice* device, const VkMemoryRequirements& memReqs, VkFlags memoryFlags): 
    m_device(device),
    m_size(memReqs.size),
    m_flags(memoryFlags)
    {
        // Get memory requirements including size, alignment and memory type
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

		allocInfo.allocationSize = memReqs.size;
		// Get the memory type index that supports host visible memory access
		// Most implementations offer multiple memory types and selecting the correct one to allocate memory from is crucial
		// We also want the buffer to be host coherent so we don't have to flush (or sync after every update.
		// Note: This may affect performance so you might not want to do this in a real world application that updates buffers on a regular base
		allocInfo.memoryTypeIndex = getMemoryTypeIndex(memReqs.memoryTypeBits, memoryFlags);
		
        // Allocate memory for the uniform buffer
		vkAllocateMemory(device->Handle(), &allocInfo, nullptr, &handle);
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
    VkDeviceSize m_size;
    VkFlags m_flags;
};