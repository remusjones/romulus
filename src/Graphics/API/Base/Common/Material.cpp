//
// Created by Remus on 17/12/2023.
//

#include "Material.h"
#include <stdexcept>
#include "VulkanGraphicsImpl.h"

void Material::Create(MaterialBase* baseMaterial)
{
	material = baseMaterial;
	const VkDescriptorSetLayoutCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.data(),
	};
	vkCreateDescriptorSetLayout(gGraphics->logicalDevice, &createInfo, nullptr, &layout);

	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = gGraphics->vulkanRenderer->descriptorPool;
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts = &layout;

	VkResult err = vkAllocateDescriptorSets(gGraphics->logicalDevice, &alloc_info, &descriptorSet);

	if (err == VK_ERROR_OUT_OF_POOL_MEMORY)
	{
		throw std::runtime_error("Out of pool memory");
	}
}

void Material::CreateProperties(const uint32_t inBinding, const MaterialProperties& inMaterialProperties)
{
	materialProperties = inMaterialProperties;
	propertiesBuffer  = AllocatedBuffer();
	propertiesBuffer.Create(sizeof(MaterialProperties), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

	for (int i = 0; i < RomulusVulkanRenderer::MAX_FRAMES_IN_FLIGHT; i++)
		SetBuffers(propertiesBuffer, inBinding, 0);
}

void Material::BindTexture(const eastl::vector<VkDescriptorImageInfo>& textureInfo, const uint8_t aBinding) const
{
	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet          = GetDescriptorSet();
	writeDescriptorSet.dstBinding      = aBinding;
	writeDescriptorSet.dstArrayElement = 0; // starting from the first array element
	writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSet.descriptorCount = static_cast<uint32_t>(textureInfo.size());
	// setting to the total number of textures in the array
	writeDescriptorSet.pImageInfo = textureInfo.data(); // pointer to the images' information array

	vkUpdateDescriptorSets(gGraphics->logicalDevice, 1, &writeDescriptorSet, 0, nullptr);
}

void Material::BindTexture(const VkDescriptorImageInfo& imageInfo, const uint8_t inBinding) const
{
	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet          = descriptorSet;
	writeDescriptorSet.dstBinding      = inBinding;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.pImageInfo      = &imageInfo;

	vkUpdateDescriptorSets(gGraphics->logicalDevice, 1, &writeDescriptorSet, 0, nullptr);
}

void Material::AddBinding(const uint32_t inBinding, const uint32_t inCount,
                          const VkDescriptorType inType, VkShaderStageFlagBits inShaderStage)
{
	VkDescriptorSetLayoutBinding binding{};
	binding.binding         = inBinding;
	binding.descriptorCount = inCount;
	binding.stageFlags      = inShaderStage;
	binding.descriptorType  = inType;
	bindings.push_back(binding);
}

void Material::SetBuffers(const AllocatedBuffer& inBuffer, const uint8_t inBinding, const uint8_t inIndex) const
{
	const VkDescriptorSetLayoutBinding& binding = bindings[inBinding];

	auto* descriptors = static_cast<VkDescriptorBufferInfo*>(alloca(
		sizeof(VkDescriptorBufferInfo) * binding.descriptorCount));
	for (int i = 0; i < binding.descriptorCount; i++)
	{
		descriptors[i].buffer = inBuffer.GetBuffer();
		descriptors[i].offset = 0;
		descriptors[i].range  = VK_WHOLE_SIZE;
	}

	VkWriteDescriptorSet writeDescription = {};
	writeDescription.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescription.dstSet               = descriptorSet;
	writeDescription.dstBinding           = inBinding;
	writeDescription.descriptorCount      = binding.descriptorCount;

	writeDescription.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescription.pBufferInfo    = descriptors;

	vkUpdateDescriptorSets(gGraphics->logicalDevice, 1, &writeDescription, 0, nullptr);
}

void Material::Destroy()
{
	vkDestroyDescriptorSetLayout(gGraphics->logicalDevice, layout, nullptr);
	vkFreeDescriptorSets(gGraphics->logicalDevice, gGraphics->vulkanRenderer->descriptorPool, 1, &descriptorSet);

	if (propertiesBuffer.IsAllocated())
	{
		propertiesBuffer.Destroy();
	}
}
