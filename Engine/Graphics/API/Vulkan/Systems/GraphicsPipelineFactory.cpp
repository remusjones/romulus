//
// Created by Remus on 9/01/2024.
//

#include "GraphicsPipelineFactory.h"
#include "VulkanGraphicsImpl.h"

void GraphicsPipelineFactory::Create(const std::vector<VkDescriptorSetLayout>& descriptorLayouts)
{
	boundDescriptorLayouts = descriptorLayouts;
	CreatePipelineLayout();
	CreatePipeline();
}

void GraphicsPipelineFactory::CreatePipelineObject(const char* pipelineName)
{
	graphicsPipeline = std::make_unique<GraphicsPipeline>(pipelineName, pipelineConfig);
}
