#pragma once

#include "vulkan/vulkan.h"
#include "base/vk/vkinstance.h"

class VKSurface: public VKObject<VkSurfaceKHR>{
public:
    VKSurface(VKInstance* instance, void* hwnd){
        
        VkWin32SurfaceCreateInfoKHR sci;
        PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;

        vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr(instance->Handle(), "vkCreateWin32SurfaceKHR");
        if (!vkCreateWin32SurfaceKHR)
        {
            return;
        }

        memset(&sci, 0, sizeof(sci));
        sci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        sci.hinstance = GetModuleHandle(NULL);
        sci.hwnd = reinterpret_cast<HWND>(hwnd);

        vkCreateWin32SurfaceKHR(instance->Handle(), &sci, nullptr, &handle);
    }
};