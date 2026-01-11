//
// Created by Remus on 17/12/2023.
//

#include "MaterialBase.h"
#include "VulkanGraphicsImpl.h"

void MaterialBase::Destroy()
{
}

void MaterialBase::AddBinding(const uint32_t inBinding, const uint32_t inCount,
	const VkDescriptorType inDescriptorType,
    const VkShaderStageFlagBits inStageFlags)
{
	VkDescriptorSetLayoutBinding binding = {};
	binding.binding                      = inBinding;
	binding.descriptorCount              = inCount;
	// todo: we don't use the inDescriptorType?
	binding.descriptorType               = static_cast<VkDescriptorType>(0 << 1);
	binding.stageFlags                   = inStageFlags;
	bindings.push_back(binding);
}
