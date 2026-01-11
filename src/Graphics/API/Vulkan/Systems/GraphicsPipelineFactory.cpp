//
// Created by Remus on 9/01/2024.
//

#include "GraphicsPipelineFactory.h"
#include "VulkanGraphicsImpl.h"

void GraphicsPipelineFactory::Create(const eastl::vector<VkDescriptorSetLayout>& descriptorLayouts)
{
	boundDescriptorLayouts = descriptorLayouts;
	CreatePipelineLayout();
	CreatePipeline();
}

void GraphicsPipelineFactory::CreatePipelineObject(const eastl::string_view pipelineName)
{
	graphicsPipeline = eastl::make_unique<GraphicsPipeline>(pipelineName, pipelineConfig);
}
