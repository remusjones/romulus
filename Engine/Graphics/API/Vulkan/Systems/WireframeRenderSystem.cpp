#include "WireframeRenderSystem.h"
#include "VulkanGraphicsImpl.h"
#include "Base/Common/Buffers/PushConstants.h"
#include "spdlog/spdlog.h"

void WireframeRenderSystem::CreatePipelineLayout()
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
        SPDLOG_ERROR("Failed to create Render System Layout 'WireframeRenderSystem'");
    }
}

void WireframeRenderSystem::CreatePipeline()
{
    assert(pipelineLayout != nullptr);

    GraphicsPipeline::DefaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = gGraphics->GetRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipelineConfig.subpass = static_cast<uint32_t>(GraphicsPipeline::SubPasses::SUBPASS_GEOMETRY);
    pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    pipelineConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;

    pipelineConfig.depthStencilInfo.depthTestEnable = VK_TRUE;
    pipelineConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;

    // create a pipeline
    CreatePipelineObject("WireframeRenderSystem");

    graphicsPipeline->CreateShaderModule("/Assets/Shaders/Unlit_v.spv", VK_SHADER_STAGE_VERTEX_BIT);
    graphicsPipeline->CreateShaderModule("/Assets/Shaders/Unlit_f.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
}
