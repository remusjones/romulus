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
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(boundDescriptorLayouts.size());
    pipelineLayoutInfo.pSetLayouts = boundDescriptorLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(gGraphics->logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
        VK_SUCCESS) {
        Logger::Log(spdlog::level::critical, "Failed to create SkyboxRenderSystem layout");
    }
}

void SkyboxRenderSystem::CreatePipeline() {
    assert(pipelineLayout != nullptr);


    GraphicsPipeline::DefaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = gGraphics->swapChain->renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipelineConfig.subpass = static_cast<uint32_t>(GraphicsPipeline::SubPasses::SUBPASS_TRANSPARENCY);
    pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_FRONT_BIT;

    pipelineConfig.depthStencilInfo.depthTestEnable = false;
    pipelineConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_NEVER;

    pipelineConfig.mAttributeDescriptions = Vertex::GetAttributeDescriptions(
        Vertex::GetBindingMask(true, false, false, false, false)
    );

    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    bindingDescriptions.push_back(Vertex::GetBindingDescription());
    pipelineConfig.mBindingDescriptions = bindingDescriptions;

    CreatePipelineObject("SkyboxRenderSystem");
    graphicsPipeline->CreateShaderModule("/Assets/Shaders/Filtercube_v.spv", VK_SHADER_STAGE_VERTEX_BIT);
    graphicsPipeline->CreateShaderModule("/Assets/Shaders/Skybox_f.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
}
