#pragma once

#include "base/utils.h"
#include "vkdevice.h"
#include "vksurface.h"
#include "vkimage.h"
#include "vkimageview.h"
#include "vkcommandbuffer.h"
#include "vkcommandbufferpool.h"
#include "vksync.h"
#include "vkframebuffer.h"

#include <cassert>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

/**
 * Physical device (GPU) that Vulkan will use
*/
class VKSwapChain: public VKObject<VkSwapchainKHR>{

public:
    VKSwapChain(VKDevice* device, VKSurface* surface, uint32_t width, uint32_t height):
    m_device(device)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device->PhysicalDevice(),surface);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR   presentMode   = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D         extent        = chooseSwapExtent(swapChainSupport.capabilities,width,height);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface          = surface->Handle();
        createInfo.minImageCount    = imageCount;
        createInfo.imageFormat      = surfaceFormat.format;
        createInfo.imageColorSpace  = surfaceFormat.colorSpace;
        createInfo.imageExtent      = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queueFamilyIndices[] = {
            device->PhysicalDevice()->GraphicsQueueFamily(), 
            device->PhysicalDevice()->PresentQueueFamily()
        };

        if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform     = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode      = presentMode;
        createInfo.clipped          = VK_TRUE;
        createInfo.oldSwapchain     = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device->Handle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        m_imageFormat   = surfaceFormat.format;
        m_extent        = extent;

        CreateImages();
    }

    ~VKSwapChain(){
        for(auto imageView: m_imageViews){
            SAFE_DELETE(imageView);
        }

        for(auto buffer: m_commandBuffers){
            VKCommandBufferPool::Instance(m_device)->FreeCommandBuffer(buffer->Handle());;
        }

        vkDestroySwapchainKHR(m_device->Handle(),handle,nullptr);
    }

    void CreateImageViews()
    {
        uint32_t imageCount = (uint32_t)m_images.size();

        for(uint32_t i = 0; i < imageCount; i++){
            m_imageViews.push_back(new VKImageView(m_device,m_images[i],m_imageFormat));
        }
    }


    void CreateCommandBuffers()
    {
        uint32_t imageCount = (uint32_t)m_images.size();

        VKCommandBufferPool* pool = VKCommandBufferPool::Instance(m_device);
        m_commandBuffers = pool->AllocateCommandBuffers(imageCount);
    }

    void CreateFrameBuffers(VKRenderPass* renderPass){
        // Create a frame buffer for every image in the swapchain
		uint32_t imageCount = (uint32_t)m_images.size();

		for (uint32_t i = 0; i < imageCount; i++)
		{
            m_frameBuffers.push_back(new VKFrameBuffer(renderPass,{m_imageViews[i]->Handle()},m_extent));
		}
    }

    VkFormat ImageFormat() const{
        return m_imageFormat;
    }

    VkExtent2D Extent2D() const{
        return m_extent;
    }

    const VKFrameBuffer* FrameBuffer(int index) const{
        assert(index < m_frameBuffers.size());
        return m_frameBuffers[index];
    }

    const VKCommandBuffer* CommandBuffer(int index) const{
        assert(index < m_commandBuffers.size());
        return m_commandBuffers[index];
    }

    const VKImageView* ImageView(int index) const{
        assert(index < m_imageViews.size());
        return m_imageViews[index];
    }

    const VKImage* Image(int index) const{
        assert(index < m_images.size());
        return m_images[index];
    }

    size_t ImageCount() const{
        return m_imageViews.size();
    }

private:
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {
        #undef max
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent = {
                width,
                height
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    SwapChainSupportDetails querySwapChainSupport(VKPhysicalDevice* physicalDevice, VKSurface* surface) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->Handle(), surface->Handle(), &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->Handle(), surface->Handle(), &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->Handle(), surface->Handle(), &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->Handle(), surface->Handle(), &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->Handle(), surface->Handle(), &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    void CreateImages()
    {
        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR(m_device->Handle(), handle, &imageCount, nullptr);

        std::vector<VkImage> images(imageCount);
        vkGetSwapchainImagesKHR(m_device->Handle(), handle, &imageCount, images.data());

        for(auto image: images){
            m_images.push_back(new VKImage(m_device, image));
        }
    }

    std::vector<VKImage*>         m_images;
    std::vector<VKImageView*>     m_imageViews;
    std::vector<VKCommandBuffer*> m_commandBuffers;
    std::vector<VKFrameBuffer*>   m_frameBuffers;

    VkFormat                    m_imageFormat;
    VkExtent2D                  m_extent;

    VKDevice*                   m_device = nullptr;
};