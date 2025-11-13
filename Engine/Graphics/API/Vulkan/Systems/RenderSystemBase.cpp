//
// Created by Remus on 9/01/2024.
//

#include "RenderSystemBase.h"

#include <Logger.h>
#include "VulkanGraphicsImpl.h"

void RenderSystemBase::Create(const std::vector<VkDescriptorSetLayout> &aDescriptorLayouts) {
    mBoundDescriptorLayouts = aDescriptorLayouts;
    CreatePipelineLayout();
    CreatePipeline();
}

void RenderSystemBase::CreatePipelineObject(const char *pipelineName, bool aDestructive) {
    if (m_graphicsPipeline == nullptr || aDestructive)
        m_graphicsPipeline = std::make_unique<GraphicsPipeline>(pipelineName, mPipelineConfig);
    else m_graphicsPipeline->pipelineConfig = mPipelineConfig;
}
