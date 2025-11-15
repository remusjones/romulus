//
// Created by Remus on 9/01/2024.
//

#include "RenderSystemBase.h"

#include <Logger.h>
#include "VulkanGraphicsImpl.h"

void RenderSystemBase::Create(const std::vector<VkDescriptorSetLayout>& descriptorLayouts)
{
	boundDescriptorLayouts = descriptorLayouts;
	CreatePipelineLayout();
	CreatePipeline();
}

void RenderSystemBase::CreatePipelineObject(const char* pipelineName)
{
	assert(graphicsPipeline == nullptr);
	graphicsPipeline = std::make_unique<GraphicsPipeline>(pipelineName, pipelineConfig);
}
