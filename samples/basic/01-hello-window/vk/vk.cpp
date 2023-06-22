#include "vulkan/vulkan.h"

#include "base/app.h"
#include "base/vk/vkinstance.h"
#include "base/vk/vkphysicaldevice.h"
#include "base/vk/vkdevice.h"
#include "base/vk/vksurface.h"
#include "base/vk/vkswapchain.h"
#include "base/vk/vkrenderpass.h"
#include "base/vk/vkpipeline.h"
#include "base/vk/vkframebuffer.h"
#include "base/vk/vkcommandbuffer.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

class VulkanExample: public App, public ICallbacks{
public:
    public:
    void RenderSceneCB() override
    {
        
        // render
        // ------
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(m_device->Handle(), m_swapchain->Handle(), UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &imageIndex);

        vkResetCommandBuffer(m_commandBuffer->Handle(), /*VkCommandBufferResetFlagBits*/ 0);
        RecordCommandBuffer(m_commandBuffer->Handle(), imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = m_commandBuffer->pHandle();

        if (vkQueueSubmit(m_device->m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        VkSwapchainKHR swapChains[] = {m_swapchain->Handle()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(m_device->m_presentQueue, &presentInfo);

        BackendSwapBuffers();
    }

    void FramebufferSizeCB(int width, int height) override
    {
        // glViewport(0, 0, width, height);
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
        CreateBasicObjects();
        CreateRenderPass();
        CreateFrameBuffer();
    }

    ~VulkanExample(){
        SAFE_DELETE(m_instance);
        SAFE_DELETE(m_physicalDevice);
    }

    void CreateBasicObjects(){
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

        m_commandBuffer = new VKCommandBuffer(m_device);
        DEV_INFO("create commandBuffer success!");
    }

    void CreateRenderPass(){
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_swapchain->GetImageFormat();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        m_renderpass = new VKRenderPass(m_device, {colorAttachment}, {subpass}, {dependency});
        DEV_INFO("create renderpass success!");
    }

    void CreateFrameBuffer(){
        auto imageviews = {
            m_swapchain->GetImageView(0)
        };
        m_framebuffer = new VKFrameBuffer(m_renderpass, imageviews, m_swapchain->GetExtent2D());
        DEV_INFO("create framebuffer success!");
    }

    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderpass->Handle();
        renderPassInfo.framebuffer = m_framebuffer->Handle();
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_swapchain->GetExtent2D();

        VkClearValue clearColor = {{{0.3f, 0.2f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_swapchain->GetExtent2D().width);
        viewport.height = static_cast<float>(m_swapchain->GetExtent2D().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_swapchain->GetExtent2D();
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }


private:
    VKInstance* m_instance = nullptr;
    VKPhysicalDevice* m_physicalDevice = nullptr;
    VKDevice* m_device = nullptr;
    VKSurface* m_surface = nullptr;
    VKSwapChain* m_swapchain = nullptr;
    VKRenderPass* m_renderpass = nullptr;
    VKGraphicsPipeline* m_pipeline = nullptr;
    VKFrameBuffer* m_framebuffer = nullptr;
    VKCommandBuffer* m_commandBuffer = nullptr;
};

int main(int argc, char** argv)
{
    BackendInit(argc,argv, API_TYPE_VK);
    BackendCreateWindow(SCR_WIDTH,SCR_HEIGHT,false/*isFullScreen*/,"Learn Vulkan");

    BackendRun(new VulkanExample);

    BackendTerminate();
    return 0;
}