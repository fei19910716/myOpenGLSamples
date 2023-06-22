#pragma once

#include "base/utils.h"
#include "vkrenderpass.h"


#include <vector>
#include <cassert>
#include <fstream>

class VKGraphicsPipeline: public VKObject<VkPipeline>{

public:
    VKGraphicsPipeline(VKDevice* device){
        // Input assembly state describes how primitives are assembled
		// This pipeline will assemble vertex data as a triangle lists (though we only use one triangle)
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
		inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		// Rasterization state
		VkPipelineRasterizationStateCreateInfo rasterizationState = {};
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationState.cullMode = VK_CULL_MODE_NONE;
		rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationState.depthClampEnable = VK_FALSE;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.depthBiasEnable = VK_FALSE;
		rasterizationState.lineWidth = 1.0f;

		// Color blend state describes how blend factors are calculated (if used)
		// We need one blend attachment state per color attachment (even if blending is not used)
		VkPipelineColorBlendAttachmentState blendAttachmentState[1] = {};
		blendAttachmentState[0].colorWriteMask = 0xf;
		blendAttachmentState[0].blendEnable = VK_FALSE;
		VkPipelineColorBlendStateCreateInfo colorBlendState = {};
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = blendAttachmentState;

		// Viewport state sets the number of viewports and scissor used in this pipeline
		// Note: This is actually overridden by the dynamic states (see below)
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// Enable dynamic states
		// Most states are baked into the pipeline, but there are still a few dynamic states that can be changed within a command buffer
		// To be able to change these we need do specify which dynamic states will be changed using this pipeline. Their actual states are set later on in the command buffer.
		// For this example we will set the viewport and scissor using dynamic states
		std::vector<VkDynamicState> dynamicStateEnables;
		dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.pDynamicStates = dynamicStateEnables.data();
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

		// Depth and stencil state containing depth and stencil compare and test operations
		// We only use depth tests and want depth tests and writes to be enabled and compare with less or equal
		VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.depthTestEnable = VK_TRUE;
		depthStencilState.depthWriteEnable = VK_TRUE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
		depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
		depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.front = depthStencilState.back;

		// Multi sampling state
		// This example does not make use of multi sampling (for anti-aliasing), the state must still be set and passed to the pipeline
		VkPipelineMultisampleStateCreateInfo multisampleState = {};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.pSampleMask = nullptr;

		// Vertex input descriptions
		// Specifies the vertex input parameters for a pipeline
		// Vertex input state used for pipeline creation
		VkPipelineVertexInputStateCreateInfo vertexInputState = {};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputState.vertexBindingDescriptionCount = 0;
		vertexInputState.pVertexBindingDescriptions = nullptr;
		vertexInputState.vertexAttributeDescriptionCount = 0;
		vertexInputState.pVertexAttributeDescriptions = nullptr;

        VkPipelineLayout pipelineLayout;
        VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
		pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pPipelineLayoutCreateInfo.pNext = nullptr;
		pPipelineLayoutCreateInfo.setLayoutCount = 0;
		pPipelineLayoutCreateInfo.pSetLayouts = nullptr;

		assert(vkCreatePipelineLayout(device->Handle(), &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout) == VK_SUCCESS);

        
		// Shaders
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};

        VkPipelineShaderStageCreateInfo vertexShader = {};
		// Vertex shader
		vertexShader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		// Set pipeline stage for this shader
		vertexShader.stage = VK_SHADER_STAGE_VERTEX_BIT;
		// Load binary SPIR-V shader
		vertexShader.module = loadSPIRVShader(device, "shaders/triangle.vert.spv");
		// Main entry point for the shader
		vertexShader.pName = "main";
		assert(vertexShader.module != VK_NULL_HANDLE);
        shaderStages.push_back(vertexShader);

        VkPipelineShaderStageCreateInfo fragShader = {};
		// Fragment shader
		fragShader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		// Set pipeline stage for this shader
		fragShader.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		// Load binary SPIR-V shader
		fragShader.module = loadSPIRVShader(device, "shaders/triangle.frag.spv");
		// Main entry point for the shader
		fragShader.pName = "main";
		assert(fragShader.module != VK_NULL_HANDLE);
        shaderStages.push_back(fragShader);

        VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		// Set pipeline shader stage info
		pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineCreateInfo.pStages = shaderStages.data();
        pipelineCreateInfo.layout = pipelineLayout;
		pipelineCreateInfo.pVertexInputState = nullptr;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineCreateInfo.pRasterizationState = &rasterizationState;
		pipelineCreateInfo.pColorBlendState = &colorBlendState;
		pipelineCreateInfo.pMultisampleState = &multisampleState;
		pipelineCreateInfo.pViewportState = &viewportState;
		pipelineCreateInfo.pDepthStencilState = &depthStencilState;
		pipelineCreateInfo.pDynamicState = &dynamicState;

        VkResult res = vkCreateGraphicsPipelines(device->Handle(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &handle);
        if( res != VK_SUCCESS){
            DEV_ERROR("pipeline error:%d",res);
        }
    }

    VkShaderModule loadSPIRVShader(VKDevice* device, std::string filename)
	{
		size_t shaderSize;
		char* shaderCode = NULL;

		std::ifstream is(filename, std::ios::binary | std::ios::in | std::ios::ate);

		if (is.is_open())
		{
			shaderSize = is.tellg();
			is.seekg(0, std::ios::beg);
			// Copy file contents into a buffer
			shaderCode = new char[shaderSize];
			is.read(shaderCode, shaderSize);
			is.close();
			assert(shaderSize > 0);
		}
		if (shaderCode)
		{
			// Create a new shader module that will be used for pipeline creation
			VkShaderModuleCreateInfo moduleCreateInfo{};
			moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleCreateInfo.codeSize = shaderSize;
			moduleCreateInfo.pCode = (uint32_t*)shaderCode;

			VkShaderModule shaderModule;
			vkCreateShaderModule(device->Handle(), &moduleCreateInfo, NULL, &shaderModule);

			delete[] shaderCode;

			return shaderModule;
		}
		else
		{
			std::cerr << "Error: Could not open shader file \"" << filename << "\"" << std::endl;
			return VK_NULL_HANDLE;
		}
	}
};