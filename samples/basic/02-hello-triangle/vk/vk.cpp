
#include "base/app.h"
#include "base/vk/vkinstance.h"
#include "base/vk/vkphysicaldevice.h"
#include "base/vk/vkdevice.h"
#include "base/vk/vksurface.h"
#include "base/vk/vkswapchain.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

class VulkanExample: public App, public ICallbacks{
public:

    void RenderSceneCB() override
    {
        
        // render
        // ------
        uint ImageIndex = 0;
        assert(vkAcquireNextImageKHR(m_device->Handle(), m_swapchain->Handle(), UINT64_MAX, NULL, NULL, &ImageIndex) == VK_SUCCESS);
    
        VkSubmitInfo submitInfo = {};
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = m_swapchain->CommandBuffer(ImageIndex)->pHandle();
        assert(vkQueueSubmit(m_device->m_graphicsQueue, 1, &submitInfo, NULL) == VK_SUCCESS);    
        
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = m_swapchain->pHandle();
        presentInfo.pImageIndices      = &ImageIndex;
        assert(vkQueuePresentKHR(m_device->m_presentQueue, &presentInfo) == VK_SUCCESS);  

        BackendSwapBuffers();
    }

    void FramebufferSizeCB(int width, int height) override
    {
        // glViewport(0, 0, width, height);

        // SAFE_DELETE(m_swapchain);

        // m_swapchain = new VKSwapChain(m_device, m_surface, width, height);
        // DEV_INFO("create swapchain success!");
    }

    void KeyboardCB(KEY Key, KEY_STATE KeyState = KEY_STATE_PRESS) override
    {
        switch (Key)
        {
            case KEY_ESCAPE:
                BackendLeaveMainLoop();
                break;
            
            default:
                break;
        }
    }


    VulkanExample(){
        CreateVulkanObjects();
        RecordCommandBuffers();
    }

    ~VulkanExample(){
        SAFE_DELETE(m_instance);
        SAFE_DELETE(m_physicalDevice);
        SAFE_DELETE(m_surface);
        SAFE_DELETE(m_device);
        SAFE_DELETE(m_swapchain);
    }

    void CreateVulkanObjects(){
        m_instance = new VKInstance(false);
        DEV_INFO("create instance success!");

        m_surface = new VKSurface(m_instance,BackendWindowHandle());
        DEV_INFO("create surface success!");

        m_physicalDevice = new VKPhysicalDevice(m_instance, m_surface);
        DEV_INFO("create physical device success!");

        m_device = new VKDevice(m_physicalDevice);
        DEV_INFO("create device success!");

        m_swapchain = new VKSwapChain(m_device,m_surface,SCR_WIDTH,SCR_HEIGHT);
        DEV_INFO("create swapchain success!");
    }

    
    void RecordCommandBuffers() {
        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        };
        
        VkClearColorValue clearColor = { 0.2f, 0.3f, 0.3f, 1.0f };
        VkClearValue clearValue = {
            .color = clearColor
        };
        
        VkImageSubresourceRange imageRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1
        };
            
        for (uint i = 0 ; i < m_swapchain->ImageCount() ; i++) {
            auto commandBuffer = m_swapchain->CommandBuffer(i)->Handle();

            vkBeginCommandBuffer(commandBuffer, &beginInfo);

                vkCmdClearColorImage(commandBuffer, m_swapchain->Image(i)->Handle(), VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &imageRange);

            vkEndCommandBuffer(commandBuffer);
        }
        
        DEV_INFO("Command buffers record success!");
    }

private:
    VKInstance*         m_instance = nullptr;
    VKSurface*          m_surface = nullptr;
    VKPhysicalDevice*   m_physicalDevice = nullptr;
    VKDevice*           m_device = nullptr;
    VKSwapChain*        m_swapchain = nullptr;
};

int main(int argc, char** argv)
{
    BackendInit(argc,argv, API_TYPE_VK);
    BackendCreateWindow(SCR_WIDTH,SCR_HEIGHT,false/*isFullScreen*/,"Learn Vulkan");

    BackendRun(new VulkanExample);

    BackendTerminate();
    return 0;
}