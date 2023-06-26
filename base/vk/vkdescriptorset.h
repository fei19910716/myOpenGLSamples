#pragma once

#include "base/utils.h"
#include "vkdevice.h"

#include <vector>
#include <cassert>


class VKDescriptorSetLayout: public VKObject<VkDescriptorSetLayout>{
public:
    VKDescriptorSetLayout(VKDevice* device, const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings)
    {
		VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
		descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayout.pNext = nullptr;
		descriptorLayout.bindingCount = (uint32_t)layoutBindings.size();
		descriptorLayout.pBindings = layoutBindings.data();

		vkCreateDescriptorSetLayout(device->Handle(), &descriptorLayout, nullptr, &handle);
    }
};

class VKDescriptorSet: public VKObject<VkDescriptorSet>{
public:
    VKDescriptorSet(VKDevice* device, const VkDescriptorSet& set):
    m_device(device)
    {
        this->handle = set;
    }

    void UpdateUBO(uint32_t binding, const VkDescriptorBufferInfo& bufferInfo) const {
        
        VkWriteDescriptorSet writeDescriptorSet = {};
		// Binding 0 : Uniform buffer
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = handle;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSet.pBufferInfo = &bufferInfo;
		// Binds this uniform buffer to binding point 0
		writeDescriptorSet.dstBinding = binding;

		vkUpdateDescriptorSets(m_device->Handle(), 1, &writeDescriptorSet, 0, nullptr);
    }

	void UpdateImage(uint32_t binding, const VkDescriptorImageInfo& imageInfo) const {
        
        VkWriteDescriptorSet descriptorSetWrite{};

        descriptorSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorSetWrite.dstSet = handle;
        descriptorSetWrite.dstBinding = binding;
        descriptorSetWrite.dstArrayElement = 0;
        descriptorSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorSetWrite.descriptorCount = 1;
        descriptorSetWrite.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_device->Handle(), 1, &descriptorSetWrite, 0, nullptr);
    }

    VKDevice* m_device = nullptr;
};

class VKDescriptorSetPool: public VKObject<VkDescriptorPool>{
public:
    VKDescriptorSetPool(VKDevice* device, const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets):
    m_device(device)
    {
		// Create the global descriptor pool
		// All descriptors used in this example are allocated from this pool
		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.pNext = nullptr;
		descriptorPoolInfo.poolSizeCount = (uint32_t)poolSizes.size();
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		// Set the max. number of descriptor sets that can be requested from this pool (requesting beyond this limit will result in an error)
		descriptorPoolInfo.maxSets = maxSets;

		vkCreateDescriptorPool(device->Handle(), &descriptorPoolInfo, nullptr, &handle);
    }

    VKDescriptorSet* AllocateDescriptorSets(const std::vector<VKDescriptorSetLayout*>& setLayouts)
	{
        VkDescriptorSet set;

        std::vector<VkDescriptorSetLayout> vkLayouts;
        for(auto setLayout: setLayouts){
            vkLayouts.push_back(setLayout->Handle());
        }


        // Allocate a new descriptor set from the global descriptor pool
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = handle;
		allocInfo.descriptorSetCount = (uint32_t)vkLayouts.size();
		allocInfo.pSetLayouts = vkLayouts.data();

		vkAllocateDescriptorSets(m_device->Handle(), &allocInfo, &set);


        return new VKDescriptorSet(m_device, set);
    }

    VKDevice* m_device = nullptr;
};
