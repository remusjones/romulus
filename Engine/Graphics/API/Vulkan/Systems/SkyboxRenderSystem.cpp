//
// Created by Remus on 7/01/2024.
//

#include "SkyboxRenderSystem.h"

#include "../../../../System/Logger.h"
#include "PipelineConfigInfo.h"
#include "VulkanGraphicsImpl.h"
#include "Base/Common/Buffers/PushConstants.h"

void SkyboxRenderSystem::CreatePipelineLayout() {
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
        Logger::Log(spdlog::level::critical, "Failed to create SkyboxRenderSystem layout");
    }
}

void SkyboxRenderSystem::CreatePipeline() {
    assert(mPipelineLayout != nullptr);


    GraphicsPipeline::DefaultPipelineConfigInfo(mPipelineConfig);
    mPipelineConfig.renderPass = gGraphics->swapChain->renderPass;
    mPipelineConfig.pipelineLayout = mPipelineLayout;
    mPipelineConfig.subpass = static_cast<uint32_t>(GraphicsPipeline::SubPasses::SUBPASS_TRANSPARENCY);
    mPipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_FRONT_BIT;

    mPipelineConfig.depthStencilInfo.depthTestEnable = false;
    mPipelineConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_NEVER;

    mPipelineConfig.mAttributeDescriptions = Vertex::GetAttributeDescriptions(
        Vertex::GetBindingMask(true, false, false, false, false)
    );

    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    bindingDescriptions.push_back(Vertex::GetBindingDescription());
    mPipelineConfig.mBindingDescriptions = bindingDescriptions;

    CreatePipelineObject("SkyboxRenderSystem");
    m_graphicsPipeline->CreateShaderModule("/Assets/Shaders/Filtercube_v.spv", VK_SHADER_STAGE_VERTEX_BIT);
    m_graphicsPipeline->CreateShaderModule("/Assets/Shaders/Skybox_f.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
}
