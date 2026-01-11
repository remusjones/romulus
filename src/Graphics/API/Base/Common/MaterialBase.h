//
// Created by Remus on 17/12/2023.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>

#include "EASTL/vector.h"
#include "Objects/IDebuggable.h"

class MaterialBase : public IDebuggable
{
public:
	void OnDebugGui() override {};

	virtual void Destroy();
	virtual void AddBinding(uint32_t inBinding, uint32_t inCount, VkDescriptorType inDescriptorType,
	                        VkShaderStageFlagBits inStageFlags);

	eastl::vector<VkDescriptorSetLayoutBinding> bindings;

	VkDescriptorSetLayout mLayout;
};
