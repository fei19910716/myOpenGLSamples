#pragma once

#include "base/utils.h"
#include "vkphysicaldevice.h"

#include <cassert>
#include <vector>
#include <string>


class VKDevice: public VKObject<VkDevice>{
public:
    VKDevice(VKPhysicalDevice* physicalDevice): m_physicalDevice(physicalDevice)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = physicalDevice->GraphicsQueueFamily();
        queueCreateInfo.queueCount = 1;

        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;

        VkPhysicalDeviceFeatures deviceFeatures{};
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        createInfo.enabledLayerCount = 0;


        assert(vkCreateDevice(physicalDevice->Handle(), &createInfo, nullptr, &handle) == VK_SUCCESS);

        vkGetDeviceQueue(handle, physicalDevice->GraphicsQueueFamily(), 0, &m_graphicsQueue);
        vkGetDeviceQueue(handle, physicalDevice->PresentQueueFamily(),  0, &m_presentQueue);
    }

    ~VKDevice(){
        vkDestroyDevice(handle,nullptr);
    }

    VKPhysicalDevice* PhysicalDevice() const{
        return m_physicalDevice;
    }

public:
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    VKPhysicalDevice* m_physicalDevice;

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};