#pragma once

#include "vulkan/vulkan.h"
#include "base/utils.h"
#include "vkrenderpass.h"


#include <vector>
#include <cassert>

class VKGraphicsPipeline: public VKObject<VkPipeline>{

public:
    VKGraphicsPipeline(VKRenderPass* renderpass){
        VkPipelineShaderStageCreateInfo shaderStages = {};
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};

        VkPipelineMultisampleStateCreateInfo multisampling{};

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};

        VkPipelineColorBlendStateCreateInfo colorBlending{};

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
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        assert(vkCreatePipelineLayout(renderpass->GetDevice()->Handle(), &pipelineLayoutInfo, nullptr, &m_layout) == VK_SUCCESS);

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pStages = &shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = m_layout;
        pipelineInfo.renderPass = renderpass->Handle();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        assert(vkCreateGraphicsPipelines(renderpass->GetDevice()->Handle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &handle) == VK_SUCCESS);
    }
    VkPipelineLayout m_layout;
};