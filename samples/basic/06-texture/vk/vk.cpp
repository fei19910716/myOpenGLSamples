
#include "base/app.h"
#include "base/vk/vkinstance.h"
#include "base/vk/vkphysicaldevice.h"
#include "base/vk/vkdevice.h"
#include "base/vk/vksurface.h"
#include "base/vk/vkswapchain.h"
#include "base/vk/vkvertexbuffer.h"
#include "base/vk/vkindexbuffer.h"
#include "base/vk/vkgraphicspipeline.h"
#include "base/math.h"



// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            return attributeDescriptions;
        }
    };


/**
 * 使用PushConstant更新shader uniform变量（Not UBO）
*/
class VulkanExample: public App, public ICallbacks{
public:


    void RenderSceneCB() override
    {
        vkWaitForFences(m_device->Handle(), 1, m_fence->pHandle(), VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(m_device->Handle(), m_swapchain->Handle(), UINT64_MAX, m_imageAvailable->Handle(), VK_NULL_HANDLE, &imageIndex);

        VkSubmitInfo submitInfo{};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = m_imageAvailable->pHandle();
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = m_swapchain->CommandBuffer(imageIndex)->pHandle();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = m_renderFinish->pHandle();
        vkQueueSubmit(m_device->m_graphicsQueue, 1, &submitInfo, m_fence->Handle());


        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = m_renderFinish->pHandle();
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = m_swapchain->pHandle();
        presentInfo.pImageIndices = &imageIndex;
        vkQueuePresentKHR(m_device->m_presentQueue, &presentInfo);

        BackendSwapBuffers();
    }

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

        m_fence = new VKFence(m_device);
        m_imageAvailable = new VKSemaphore(m_device);
        m_renderFinish = new VKSemaphore(m_device);

        m_swapchain = new VKSwapChain(m_device,m_surface,SCR_WIDTH,SCR_HEIGHT);
        m_swapchain->CreateImageViews();
        m_swapchain->CreateCommandBuffers();
        DEV_INFO("create swapchain success!");

        CreateRenderpass();
        m_swapchain->CreateFrameBuffers(m_renderPass);
        DEV_INFO("create renderpass success!");

        CreateVertexBuffer();
        DEV_INFO("create vertexbuffer success!");

        CreateGraphicsPipeline();
        DEV_INFO("create pipeline success!");
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

    void CreateVertexBuffer(){

        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
            {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}
        };

        const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
        };

		uint32_t vertexCount = static_cast<uint32_t>(vertices.size());
        m_VBO = new VKVertexBuffer(m_device, vertexCount * sizeof(Vertex), vertexCount);
        m_VBO->StageLoadRaw(vertices.data());

        uint32_t indexCount = static_cast<uint32_t>(indices.size());
        m_IBO = new VKIndexBuffer(m_device, indexCount * sizeof(uint16_t), indexCount);
        m_IBO->StageLoadRaw(indices.data());
    }

    void CreateGraphicsPipeline(){

        auto createShaderModule = [&](const std::vector<char>& code){
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = code.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

            VkShaderModule shaderModule;
            if (vkCreateShaderModule(m_device->Handle(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shader module!");
            }

            return shaderModule;
        };

        auto vertShaderCode = UTILS::ReadShaderFile("shaders/vk-vertex-attributes.vert.spv");
        auto fragShaderCode = UTILS::ReadShaderFile("shaders/vk-vertex-attributes.frag.spv");

        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount  = 0;

        VkPipelineLayout pipelineLayout = {};;
        if (vkCreatePipelineLayout(m_device->Handle(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
        m_pipelineLayout = new VKPipelineLayout(m_device, pipelineLayout);

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = m_renderPass->Handle();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        VkPipeline graphicsPipeline;
        if (vkCreateGraphicsPipelines(m_device->Handle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
        m_pipeline = new VKGraphicsPipeline(m_device, graphicsPipeline);

        vkDestroyShaderModule(m_device->Handle(), fragShaderModule, nullptr);
        vkDestroyShaderModule(m_device->Handle(), vertShaderModule, nullptr);
    }

    void RecordCommandBuffers() {
        VkCommandBufferBeginInfo beginInfo = {};;
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
        VkClearColorValue clearColor = { 0.0f, 0.3f, 0.0f, 1.0f };
        VkClearValue clearValue = {};;
        clearValue.color = clearColor;
        
        VkImageSubresourceRange imageRange = {};;
        imageRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageRange.levelCount = 1;
        imageRange.layerCount = 1;
            
        for (uint i = 0 ; i < m_swapchain->ImageCount() ; i++) {
            auto commandBuffer = m_swapchain->CommandBuffer(i)->Handle();

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

            vkBeginCommandBuffer(commandBuffer, &beginInfo);
                vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
                    // Update dynamic viewport state
                    VkViewport viewport{};
                    viewport.x = 0.0f;
                    viewport.y = 0.0f;
                    viewport.width = static_cast<float>(m_swapchain->Extent2D().width);
                    viewport.height = static_cast<float>(m_swapchain->Extent2D().height);
                    viewport.minDepth = 0.0f;
                    viewport.maxDepth = 1.0f;
                    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

                    VkRect2D scissor{};
                    scissor.offset = {0, 0};
                    scissor.extent = m_swapchain->Extent2D();
                    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

                    // Bind the rendering pipeline
                    // The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation time
                    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->Handle());

                    VkDeviceSize offsets[] = {0};
                    vkCmdBindVertexBuffers(commandBuffer, 0, 1, m_VBO->pHandle(), offsets);

                    vkCmdBindIndexBuffer(commandBuffer, m_IBO->Handle(), 0, VK_INDEX_TYPE_UINT16);

                    vkCmdDrawIndexed(commandBuffer, m_IBO->m_indexCount, 1, 0, 0, 0);

                vkCmdEndRenderPass(commandBuffer);
            vkEndCommandBuffer(commandBuffer);
        }
        
        DEV_INFO("Command buffers record success!");
    }

private:
    VKInstance*         m_instance = nullptr;
    VKSurface*          m_surface = nullptr;
    VKPhysicalDevice*   m_physicalDevice = nullptr;
    VKDevice*           m_device = nullptr;
    VKRenderPass*       m_renderPass = nullptr;
    VKSwapChain*        m_swapchain = nullptr;
    VKGraphicsPipeline* m_pipeline = nullptr;
    VKPipelineLayout*   m_pipelineLayout = nullptr;
    VKVertexBuffer*     m_VBO = nullptr;
    VKIndexBuffer*      m_IBO = nullptr;
    VKFence*            m_fence;
    VKSemaphore*        m_imageAvailable = nullptr, *m_renderFinish = nullptr;
};

int main(int argc, char** argv)
{
    BackendInit(argc,argv, API_TYPE_VK);
    BackendCreateWindow(SCR_WIDTH,SCR_HEIGHT,false/*isFullScreen*/,"Learn Vulkan");

    BackendRun(new VulkanExample);

    BackendTerminate();
    return 0;
}