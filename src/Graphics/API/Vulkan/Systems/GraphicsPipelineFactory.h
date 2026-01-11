//
// Created by Remus on 9/01/2024.
//

#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "GraphicsPipeline.h"
#include "EASTL/unique_ptr.h"


class GraphicsPipelineFactory
{
public:
	virtual ~GraphicsPipelineFactory() = default;
	virtual void Create(const eastl::vector<VkDescriptorSetLayout>& descriptorLayouts);

protected:
	virtual void CreatePipelineLayout() = 0;
	virtual void CreatePipeline() = 0;
	void CreatePipelineObject(const eastl::string_view pipelineName);

public:
	eastl::unique_ptr<GraphicsPipeline> graphicsPipeline;
	eastl::vector<VkDescriptorSetLayout> GetBoundDescriptors() { return boundDescriptorLayouts; }

protected:
	VkPipelineLayout pipelineLayout = nullptr;
	eastl::vector<VkDescriptorSetLayout> boundDescriptorLayouts;
	PipelineConfigInfo pipelineConfig{};
};
