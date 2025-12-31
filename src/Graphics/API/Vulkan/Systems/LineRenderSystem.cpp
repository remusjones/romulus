//
// Created by Remus on 19/01/2024.
//

#include "LineRenderSystem.h"

#include "VulkanGraphicsImpl.h"
#include "Base/Common/Buffers/PushConstants.h"
#include "spdlog/spdlog.h"

void LineRenderSystem::CreatePipelineLayout()
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
        SPDLOG_ERROR("Failed to create Render System Layout 'LineRenderSystem'");
    }
}

void LineRenderSystem::CreatePipeline()
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

    pipelineConfig.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;

    eastl::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0] = Vertex::GetBindingDescription();
    pipelineConfig.mBindingDescriptions = bindingDescriptions;

    const eastl::bitset<5> vertexMask = Vertex::GetBindingMask(true, false, true, false, false);
    pipelineConfig.mAttributeDescriptions = Vertex::GetAttributeDescriptions(vertexMask);

    // create a pipeline
    CreatePipelineObject("LineRenderSystem");

    graphicsPipeline->CreateShaderModule("/Assets/Shaders/Line_v.spv", VK_SHADER_STAGE_VERTEX_BIT);
    graphicsPipeline->CreateShaderModule("/Assets/Shaders/Line_f.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
}
