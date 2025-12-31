//
// Created by Remus on 9/01/2024.
//

#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "GraphicsPipeline.h"


class GraphicsPipelineFactory
{
public:
	virtual ~GraphicsPipelineFactory() = default;
	virtual void Create(const std::vector<VkDescriptorSetLayout>& descriptorLayouts);

protected:
	virtual void CreatePipelineLayout() = 0;
	virtual void CreatePipeline() = 0;
	void CreatePipelineObject(const char* pipelineName);

public:
	std::unique_ptr<GraphicsPipeline> graphicsPipeline;
	std::vector<VkDescriptorSetLayout> GetBoundDescriptors() { return boundDescriptorLayouts; }

protected:
	VkPipelineLayout pipelineLayout = nullptr;
	std::vector<VkDescriptorSetLayout> boundDescriptorLayouts;
	PipelineConfigInfo pipelineConfig{};
};
