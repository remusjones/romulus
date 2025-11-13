#include "WireframeRenderSystem.h"

#include <Logger.h>
#include "VulkanGraphicsImpl.h"
#include "Base/Common/Buffers/PushConstants.h"

void WireframeRenderSystem::CreatePipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstants);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(mBoundDescriptorLayouts.size());
    pipelineLayoutInfo.pSetLayouts = mBoundDescriptorLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(gGraphics->logicalDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout) !=
        VK_SUCCESS) {
        Logger::Log(spdlog::level::critical, "Failed to create <WireframeRenderSystem> render pipeline layout");
        }
}

void WireframeRenderSystem::CreatePipeline() {
    assert(mPipelineLayout != nullptr);

    GraphicsPipeline::DefaultPipelineConfigInfo(mPipelineConfig);
    mPipelineConfig.renderPass = gGraphics->swapChain->renderPass;
    mPipelineConfig.pipelineLayout = mPipelineLayout;
    mPipelineConfig.subpass = static_cast<uint32_t>(GraphicsPipeline::SubPasses::SUBPASS_GEOMETRY);
    mPipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    mPipelineConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;

    mPipelineConfig.depthStencilInfo.depthTestEnable = VK_TRUE;
    mPipelineConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;

    // create a pipeline
    CreatePipelineObject("WireframeRenderSystem");

    m_graphicsPipeline->CreateShaderModule("/Assets/Shaders/Unlit_v.spv", VK_SHADER_STAGE_VERTEX_BIT);
    m_graphicsPipeline->CreateShaderModule("/Assets/Shaders/Unlit_f.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
}