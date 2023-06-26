
#include "base/app.h"
#include "base/vk/vkinstance.h"
#include "base/vk/vkphysicaldevice.h"
#include "base/vk/vkdevice.h"
#include "base/vk/vksurface.h"
#include "base/vk/vkswapchain.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

class BaseExample: public App, public ICallbacks{
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

    virtual ~BaseExample(){
        SAFE_DELETE(m_swapchain);
        SAFE_DELETE(m_device);
        SAFE_DELETE(m_physicalDevice);
        SAFE_DELETE(m_surface);
        SAFE_DELETE(m_instance);
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
        m_swapchain->CreateImageViews();
        m_swapchain->CreateCommandBuffers();
        DEV_INFO("create swapchain success!");
    }

    virtual void RecordCommandBuffers() = 0;

protected:
    VKInstance*         m_instance = nullptr;
    VKSurface*          m_surface = nullptr;
    VKPhysicalDevice*   m_physicalDevice = nullptr;
    VKDevice*           m_device = nullptr;
    VKSwapChain*        m_swapchain = nullptr;
};

class ClearColorWithoutRenderPass: public BaseExample{
public:

    void FramebufferSizeCB(int width, int height) override
    {
        // glViewport(0, 0, width, height);

        vkDeviceWaitIdle(m_device->Handle());

        SAFE_DELETE(m_swapchain);
        m_swapchain = new VKSwapChain(m_device,m_surface,width,height);
        m_swapchain->CreateImageViews();
        m_swapchain->CreateCommandBuffers();

        RecordCommandBuffers();
    }

    ClearColorWithoutRenderPass(){
        CreateVulkanObjects();
        RecordCommandBuffers();
    }

    ~ClearColorWithoutRenderPass(){
        BaseExample::~BaseExample();
    }

    
    void RecordCommandBuffers() override {
        VkCommandBufferBeginInfo beginInfo;
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
        VkClearColorValue clearColor = { 0.0f, 0.3f, 0.0f, 1.0f };
        VkClearValue clearValue;
        clearValue.color = clearColor;
        
        VkImageSubresourceRange imageRange;
        imageRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageRange.levelCount = 1;
        imageRange.layerCount = 1;
            
        for (uint i = 0 ; i < m_swapchain->ImageCount() ; i++) {
            auto commandBuffer = m_swapchain->CommandBuffer(i)->Handle();

            vkBeginCommandBuffer(commandBuffer, &beginInfo);

                vkCmdClearColorImage(commandBuffer, m_swapchain->Image(i)->Handle(), VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &imageRange);

            vkEndCommandBuffer(commandBuffer);
        }
        
        DEV_INFO("Command buffers record success!");
    }
};

class ClearColorWithRenderPass: public BaseExample{
public:

    void FramebufferSizeCB(int width, int height) override
    {
        // glViewport(0, 0, width, height);

        vkDeviceWaitIdle(m_device->Handle());

        SAFE_DELETE(m_swapchain);
        m_swapchain = new VKSwapChain(m_device,m_surface,width,height);
        m_swapchain->CreateImageViews();
        m_swapchain->CreateFrameBuffers(m_renderPass);
        m_swapchain->CreateCommandBuffers();

        RecordCommandBuffers();
    }

    ClearColorWithRenderPass(){
        CreateVulkanObjects();

        CreateRenderpass();
        m_swapchain->CreateFrameBuffers(m_renderPass);
        DEV_INFO("create renderpass success!");

        RecordCommandBuffers();
    }

    ~ClearColorWithRenderPass(){
        BaseExample::~BaseExample();
        SAFE_DELETE(m_renderPass);
    }

    
    void CreateRenderpass(){
        VkAttachmentDescription attachment = {};

		// Color attachment
        attachment.format = m_swapchain->ImageFormat();                                  // Use the color format selected by the swapchain
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;                                 // We don't use multi sampling in this example
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                            // Clear this attachment at the start of the render pass
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;                          // Keep its contents after the render pass is finished (for displaying it)
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;                 // We don't use stencil, so don't care for load
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;               // Same for store
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                       // Layout at render pass start. Initial doesn't matter, so we use undefined
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;                   // Layout to which the attachment is transitioned when the render pass is finished
		                                                                                // As we want to present the color buffer to the swapchain, we transition to PRESENT_KHR

		// Setup attachment references
		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;                                    // Attachment 0 is color
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Attachment layout used as color during the subpass

		// Setup a single subpass reference
		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;                            // Subpass uses one color attachment
		subpassDescription.pColorAttachments = &colorReference;                 // Reference to the color attachment in slot 0
		subpassDescription.pDepthStencilAttachment = nullptr;           // Reference to the depth attachment in slot 1
		subpassDescription.inputAttachmentCount = 0;                            // Input attachments can be used to sample from contents of a previous subpass
		subpassDescription.pInputAttachments = nullptr;                         // (Input attachments not used by this example)
		subpassDescription.preserveAttachmentCount = 0;                         // Preserved attachments can be used to loop (and preserve) attachments through subpasses
		subpassDescription.pPreserveAttachments = nullptr;                      // (Preserve attachments not used by this example)
		subpassDescription.pResolveAttachments = nullptr;                       // Resolve attachments are resolved at the end of a sub pass and can be used for e.g. multi sampling

		// Setup subpass dependencies
		// These will add the implicit attachment layout transitions specified by the attachment descriptions
		// The actual usage layout is preserved through the layout specified in the attachment reference
		// Each subpass dependency will introduce a memory and execution dependency between the source and dest subpass described by
		// srcStageMask, dstStageMask, srcAccessMask, dstAccessMask (and dependencyFlags is set)
		// Note: VK_SUBPASS_EXTERNAL is a special constant that refers to all commands executed outside of the actual renderpass)
		VkSubpassDependency dependency;

		// Color attachment
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		dependency.dependencyFlags = 0;

		m_renderPass = new VKRenderPass(m_device,{attachment},{subpassDescription},{dependency});
    }
    
    void RecordCommandBuffers() override {
        VkCommandBufferBeginInfo beginInfo = {};;
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
        VkClearColorValue clearColor = { 0.0f, 0.3f, 0.0f, 1.0f };
        VkClearValue clearValue = {};;
        clearValue.color = clearColor;
        
        VkImageSubresourceRange imageRange = {};
        imageRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageRange.levelCount = 1;
        imageRange.layerCount = 1;
            
        for (uint i = 0 ; i < m_swapchain->ImageCount() ; i++) {
            auto commandBuffer = m_swapchain->CommandBuffer(i)->Handle();

            vkBeginCommandBuffer(commandBuffer, &beginInfo);

                // vkCmdClearColorImage(commandBuffer, m_swapchain->Image(i)->Handle(), VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &imageRange);
                VkRenderPassBeginInfo renderPassBeginInfo = {};
                renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderPassBeginInfo.pNext = nullptr;
                renderPassBeginInfo.renderPass = m_renderPass->Handle();
                renderPassBeginInfo.renderArea.offset.x = 0;
                renderPassBeginInfo.renderArea.offset.y = 0;
                renderPassBeginInfo.renderArea.extent = m_swapchain->Extent2D();
                renderPassBeginInfo.clearValueCount = 1;
                renderPassBeginInfo.pClearValues = &clearValue;
                renderPassBeginInfo.framebuffer = m_swapchain->FrameBuffer(i)->Handle();

                vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

                vkCmdEndRenderPass(commandBuffer);
            vkEndCommandBuffer(commandBuffer);
        }
        
        DEV_INFO("Command buffers record success!");
    }

private:
    VKRenderPass*  m_renderPass = nullptr;
};

int main(int argc, char** argv)
{
    BackendInit(argc,argv, API_TYPE_VK);
    BackendCreateWindow(SCR_WIDTH,SCR_HEIGHT,false/*isFullScreen*/,"Learn Vulkan");

    BackendRun(new ClearColorWithRenderPass);
    // BackendRun(new ClearColorWithoutRenderPass);

    BackendTerminate();
    return 0;
}