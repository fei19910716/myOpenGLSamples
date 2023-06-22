#pragma once

#include "base/utils.h"
#include "vkobject.h"

#include <vector>
#include <string>
#include <cassert>


/**
 * Vulkan instance, stores all per-application states
*/
class VKInstance : public VKObject<VkInstance>{
public:
    VKInstance(bool enableValidation){
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;


        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = NULL;
        instanceCreateInfo.pApplicationInfo = &appInfo;

        auto instanceExtensions = InstanceCreationExtensions();
        instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

        assert(vkCreateInstance(&instanceCreateInfo, nullptr, &handle) == VK_SUCCESS);
    }

    ~VKInstance(){
        vkDestroyInstance(handle,nullptr);
    }

private:

    std::vector<const char*> InstanceCreationExtensions(){

        std::vector<const char*> result;
        
        // Get extensions supported by the instance and store for later use
        uint32_t extCount = 0;
        std::vector<std::string> supportedInstanceExtensions;
        vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
        if (extCount > 0)
        {
            std::vector<VkExtensionProperties> extensions(extCount);
            if (vkEnumerateInstanceExtensionProperties(nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
            {
                for (VkExtensionProperties extension : extensions)
                {
                    supportedInstanceExtensions.push_back(extension.extensionName);
                }
            }
        }
        // Enabled requested instance extensions
        if (enabledInstanceExtensions.size() > 0) 
        {
            for (const auto& enabledExtension : enabledInstanceExtensions) 
            {
                // Output message if requested extension is not available
                if (std::find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(), enabledExtension) == supportedInstanceExtensions.end())
                {
                    std::cerr << "Enabled instance extension \"" << enabledExtension << "\" is not present at instance level\n";
                }
                result.push_back(enabledExtension.c_str());
            }
        }

        return result;
    }


    std::vector<std::string> enabledInstanceExtensions{
        VK_KHR_SURFACE_EXTENSION_NAME,
                // Enable surface extensions depending on os
        #if defined(_WIN32)
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME
        #elif defined(VK_USE_PLATFORM_ANDROID_KHR)
            VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
        #endif
    };
};