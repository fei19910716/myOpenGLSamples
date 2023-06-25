#pragma once

#include "base/utils.h"
#include "vkrenderpass.h"
#include "vkdescriptorset.h"

#include <vector>
#include <cassert>
#include <fstream>
#include <string>

class VKPipelineLayout: public VKObject<VkPipelineLayout>{
public:
	~VKPipelineLayout(){
        vkDestroyPipelineLayout(m_device->Handle(), handle, nullptr);
    }

    VKPipelineLayout(VKDevice* device, const VkPipelineLayout& image): m_device(device)
    {
        this->handle = image;
    }

    VKDevice* m_device = nullptr;
};


class VKGraphicsPipeline: public VKObject<VkPipeline>{

public:
	~VKGraphicsPipeline(){
        vkDestroyPipeline(m_device->Handle(), handle, nullptr);
    }

    VKGraphicsPipeline(VKDevice* device, const VkPipeline& image): m_device(device)
    {
        this->handle = image;
    }

    VKDevice* m_device = nullptr;
};