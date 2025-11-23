//
// Created by Remus on 17/12/2023.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include "Objects/IDebugabble.h"

class MaterialBase : public IDebugabble
{
public:
	virtual void OnDebugGui()
	{
	};
	virtual void Destroy();
	virtual void AddBinding(uint32_t inBinding, uint32_t inCount, VkDescriptorType inDescriptorType,
	                        VkShaderStageFlagBits inStageFlags);

	void Create();

	std::vector<VkDescriptorSetLayoutBinding> bindings;

	VkDescriptorSetLayout mLayout;
};
