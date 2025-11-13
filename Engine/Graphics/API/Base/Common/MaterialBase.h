//
// Created by Remus on 17/12/2023.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include "Objects/ImGuiLayer.h"

class MaterialBase : public ImGuiLayer
{
public:
	virtual void OnImGuiRender()
	{
	};
	virtual void Destroy();
	virtual void AddBinding(uint32_t inBinding, uint32_t inCount, VkDescriptorType inDescriptorType,
	                        VkShaderStageFlagBits inStageFlags);

	void Create();

	std::vector<VkDescriptorSetLayoutBinding> bindings;

	VkDescriptorSetLayout mLayout;
};
