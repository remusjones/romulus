//
// Created by Remus on 9/01/2024.
//

#include "LightingRenderSystem.h"

#include "VulkanGraphicsImpl.h"
#include "Base/Common/Buffers/PushConstants.h"
#include "spdlog/spdlog.h"

LightingRenderSystem::LightingRenderSystem(const eastl::vector<VkDescriptorSetLayout>& aDescriptorLayouts)
{
    boundDescriptorLayouts = aDescriptorLayouts;
}

void LightingRenderSystem::CreatePipelineLayout()
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstants);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(boundDescriptorLayouts.size());
    pipelineLayoutInfo.pSetLayouts = boundDescriptorLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(gGraphics->logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
        VK_SUCCESS)
    {
        SPDLOG_ERROR("Failed to create Render System Layout 'LightingRenderSystem'");
    }
}

void LightingRenderSystem::CreatePipeline()
{
    assert(pipelineLayout != nullptr);

    GraphicsPipeline::DefaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = gGraphics->GetRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipelineConfig.subpass = static_cast<uint32_t>(GraphicsPipeline::SubPasses::SUBPASS_LIGHTING);

    pipelineConfig.depthStencilInfo.depthTestEnable = false;
    pipelineConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_NEVER;

    CreatePipelineObject("LightingRenderSystem");
    graphicsPipeline->CreateShaderModule("/Assets/Shaders/Filtercube.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    graphicsPipeline->CreateShaderModule("/Assets/Shaders/Skybox.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
}
