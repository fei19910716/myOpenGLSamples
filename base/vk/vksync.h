#include "base/utils.h"
#include "vkdevice.h"

#include <vector>
#include <cassert>

class VKSemaphore: public VKObject<VkSemaphore>{
public:
    VKSemaphore(VKDevice* device): m_device(device)
    {
        // Semaphores (Used for correct command ordering)
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;

		// Semaphore used to ensure that image presentation is complete before starting to submit again
		vkCreateSemaphore(m_device->Handle(), &semaphoreCreateInfo, nullptr, &handle);
    }

    ~VKSemaphore(){
        vkDestroySemaphore(m_device->Handle(), handle, nullptr);
    }

    VKDevice* m_device = nullptr;
};


class VKFence: public VKObject<VkFence>{
public:
    VKFence(VKDevice* device): m_device(device)
    {
        // Fences (Used to check draw command buffer completion)
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		// Create in signaled state so we don't wait on first render of each command buffer
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		
        vkCreateFence(m_device->Handle(), &fenceCreateInfo, nullptr, &handle);
    }

    ~VKFence(){
        vkDestroyFence(m_device->Handle(), handle, nullptr);
    }

    VKDevice* m_device = nullptr;
};



class VKEvent: public VKObject<VkEvent>{
};