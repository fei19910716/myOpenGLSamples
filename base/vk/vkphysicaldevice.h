#pragma once

#include "base/utils.h"
#include "vkinstance.h"
#include "vksurface.h"

#include <cassert>
#include <vector>
#include <string>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

/**
 * Physical device (GPU) that Vulkan will use
*/
class VKPhysicalDevice: public VKObject<VkPhysicalDevice>{
public:
    VKPhysicalDevice(VKInstance* instance, VKSurface* surface){
        // Physical device
        uint32_t gpuCount = 0;
        // Get number of available physical devices
        assert(vkEnumeratePhysicalDevices(instance->Handle(), &gpuCount, nullptr) == VK_SUCCESS);
        if (gpuCount == 0) {
            DEV_ERROR("No device with Vulkan support found!");
            return;
        }
        // Enumerate devices
        std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
        assert(vkEnumeratePhysicalDevices(instance->Handle(), &gpuCount, physicalDevices.data()) == VK_SUCCESS);

        // GPU selection

        // Select physical device to be used for the Vulkan example
        // Defaults to the first device unless specified by command line
        for (const auto& device : physicalDevices) {
            if (IsDeviceSuitable(device, surface)) {
                this->handle = device;
                break;
            }
        }

        // Store properties (including limits), features and memory properties of the physical device (so that examples can check against them)
        vkGetPhysicalDeviceProperties(this->handle, &deviceProperties);
        vkGetPhysicalDeviceFeatures(this->handle, &deviceFeatures);
        vkGetPhysicalDeviceMemoryProperties(this->handle, &deviceMemoryProperties);
    }

    ~VKPhysicalDevice(){
        
    }

    uint32_t GraphicsQueueFamily() const{
        return indices.graphicsFamily.value();
    }

    uint32_t PresentQueueFamily() const{
        return indices.presentFamily.value();
    }

public:

    bool IsDeviceSuitable(VkPhysicalDevice device, VKSurface* surface) {

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (int i = 0; i < queueFamilies.size(); i++) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface->Handle(), &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                return true;
            }
        }

        return false;
    }

    QueueFamilyIndices indices;
    
    // Stores physical device properties (for e.g. checking device limits)
	VkPhysicalDeviceProperties deviceProperties;
	// Stores the features available on the selected physical device (for e.g. checking if a feature is available)
	VkPhysicalDeviceFeatures deviceFeatures;
	// Stores all available memory (type) properties for the physical device
	VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
};