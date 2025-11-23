#include <vulkan/vulkan_core.h>
#include "GraphicsPipeline.h"

#include <stdexcept>
#include <Vulkan/Systems/PipelineConfigInfo.h>
#include <Base/Common/Data/Vertex.h>
#include <VulkanGraphicsImpl.h>
#include <FileManagement.h>
#include <Base/Common/Buffers/PushConstants.h>
#include <Base/Common/Material.h>
#include "Base/Common/Data/GPUSceneData.h"
#include <Scenes/Scene.h>
#include <Vulkan/Common/MeshObject.h>
#include <Vulkan/Helpers/VulkanInitialization.h>

#include "spdlog/spdlog.h"

void GraphicsPipeline::Create()
{
    SPDLOG_INFO("Creating Graphics Pipeline {}", pipelineName);
    if (pipelineConfig.pipelineLayout == VK_NULL_HANDLE)
    {
        VkPushConstantRange pushConstant;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(PushConstants);
        pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstant;

        // Bind these elsewhere?
        eastl::vector<VkDescriptorSetLayout> layouts;
        for (const auto& mRenderer : renderers)
        {
            layouts.push_back(mRenderer->GetMaterial(0)->GetDescriptorLayout());
        }
        //hook the global set layout
        pipelineLayoutInfo.setLayoutCount = layouts.size();
        pipelineLayoutInfo.pSetLayouts = layouts.data();


        if (vkCreatePipelineLayout(gGraphics->logicalDevice, &pipelineLayoutInfo,
                                   nullptr, &pipelineConfig.pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout");
        }


        pipelineConfig.depthStencilInfo = VulkanInitialization::DepthStencilCreateInfo(true, true,
            VK_COMPARE_OP_LESS_OR_EQUAL);
        pipelineConfig.renderPass = gGraphics->swapChain->renderPass;
    }

    // Vertex Information
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(
        pipelineConfig.mAttributeDescriptions.size());

    vertexInputInfo.pVertexBindingDescriptions = pipelineConfig.mBindingDescriptions.data();
    vertexInputInfo.pVertexAttributeDescriptions = pipelineConfig.mAttributeDescriptions.data();

    // Pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pDepthStencilState = &pipelineConfig.depthStencilInfo;
    pipelineInfo.pStages = shaders.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &pipelineConfig.inputAssemblyInfo;
    pipelineInfo.pViewportState = &pipelineConfig.viewportInfo;
    pipelineInfo.pRasterizationState = &pipelineConfig.rasterizationInfo;
    pipelineInfo.pMultisampleState = &pipelineConfig.multisampleInfo;
    pipelineInfo.pColorBlendState = &pipelineConfig.colorBlendInfo;
    pipelineInfo.pDynamicState = &pipelineConfig.dynamicStateInfo;
    pipelineInfo.layout = pipelineConfig.pipelineLayout;
    pipelineInfo.renderPass = pipelineConfig.renderPass;
    // TODO: Increase amount of subpasses
    pipelineInfo.subpass = 0; // mPipelineConfig.subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(gGraphics->logicalDevice, VK_NULL_HANDLE, 1,
                                  &pipelineInfo, nullptr,
                                  &romulusPipelineConfig) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline");
    }
}

void GraphicsPipeline::CreateShaderModule(const char* path,
                                          const VkShaderStageFlagBits stage)
{
    const constexpr char* entryName = "main";

    const eastl::vector<char> file = FileManagement::GetShaderFileDataPath(path);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = file.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(file.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(gGraphics->logicalDevice, &createInfo, nullptr,
                             &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module");
    }

    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = stage;
    shaderStageInfo.module = shaderModule;
    shaderStageInfo.pName = entryName;

    shaders.push_back(shaderStageInfo);
}

void GraphicsPipeline::Destroy()
{
    // Fast path if nothing to destroy
    if (shaders.empty() && romulusPipelineConfig == VK_NULL_HANDLE && pipelineConfig.pipelineLayout == VK_NULL_HANDLE)
    {
        return;
    }

    SPDLOG_INFO("Destroying Graphics Pipeline {}", pipelineName);

    const VkDevice device = (gGraphics != nullptr) ? gGraphics->logicalDevice : VK_NULL_HANDLE;

    // If device is invalid (e.g., already destroyed), skip Vulkan calls and just clear local state
    if (device == VK_NULL_HANDLE)
    {
        shaders.clear();
        romulusPipelineConfig = VK_NULL_HANDLE;
        pipelineConfig.pipelineLayout = VK_NULL_HANDLE;
        return;
    }

    for (auto& stageInfo : shaders)
    {
        if (stageInfo.module != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, stageInfo.module, nullptr);
            stageInfo.module = VK_NULL_HANDLE;
        }
    }

    if (pipelineConfig.pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(device, pipelineConfig.pipelineLayout, nullptr);
        pipelineConfig.pipelineLayout = VK_NULL_HANDLE;
    }
    if (romulusPipelineConfig != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(device, romulusPipelineConfig, nullptr);
        romulusPipelineConfig = VK_NULL_HANDLE;
    }

    shaders.clear();
}


void GraphicsPipeline::DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo)
{
    configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
    configInfo.inputAssemblyInfo.pNext = nullptr;
    configInfo.inputAssemblyInfo.flags = 0;

    configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    configInfo.viewportInfo.viewportCount = 1;
    configInfo.viewportInfo.pViewports = nullptr;
    configInfo.viewportInfo.scissorCount = 1;
    configInfo.viewportInfo.pScissors = nullptr;

    configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
    configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    configInfo.rasterizationInfo.lineWidth = 1.0f;
    configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
    configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
    configInfo.rasterizationInfo.depthBiasClamp = 0.0f; // Optional
    configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f; // Optional

    configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
    configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    configInfo.multisampleInfo.minSampleShading = 1.0f; // Optional
    configInfo.multisampleInfo.pSampleMask = nullptr; // Optional
    configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE; // Optional


    configInfo.colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
    configInfo.colorBlendAttachment.blendEnable = VK_TRUE;
    configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
    configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
    configInfo.colorBlendInfo.attachmentCount = 1;
    configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
    configInfo.colorBlendInfo.blendConstants[0] = 0.0f; // Optional
    configInfo.colorBlendInfo.blendConstants[1] = 0.0f; // Optional
    configInfo.colorBlendInfo.blendConstants[2] = 0.0f; // Optional
    configInfo.colorBlendInfo.blendConstants[3] = 0.0f; // Optional

    configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.minDepthBounds = 0.0f; // Optional
    configInfo.depthStencilInfo.maxDepthBounds = 1.0f; // Optional
    configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.front = {}; // Optional
    configInfo.depthStencilInfo.back = {}; // Optional

    configInfo.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
    configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
    configInfo.dynamicStateInfo.flags = 0;
    configInfo.dynamicStateInfo.pNext = VK_NULL_HANDLE;

    configInfo.subpass = static_cast<uint32_t>(GraphicsPipeline::SubPasses::SUBPASS_GEOMETRY);

    eastl::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0] = Vertex::GetBindingDescription();

    configInfo.mBindingDescriptions = bindingDescriptions;
    configInfo.mAttributeDescriptions = Vertex::GetAttributeDescriptions();
}

void GraphicsPipeline::AddRenderer(Renderer* renderer)
{
    renderers.push_back(renderer);
}

void GraphicsPipeline::Draw(VkCommandBuffer commandBuffer, const Scene& scene) const
{
    if (romulusPipelineConfig == nullptr)
    {
        throw std::runtime_error("RomulusGraphicsPipeline is null");\
    }

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      romulusPipelineConfig);

    const FrameData& currentFrame = gGraphics->vulkanRenderer->GetCurrentFrame();

    AllocatedBuffer::MapMemory(
        gGraphics->allocator, &scene.sceneData, currentFrame.sceneBuffer.GetAllocation(),
        sizeof(GPUSceneData));

    for (Renderer* const& renderer : renderers)
    {
        eastl::vector<VkDescriptorSet> descriptorSets;
        descriptorSets.push_back(renderer->GetMaterial(0)->GetDescriptorSet());

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfig.pipelineLayout, 0,
                                descriptorSets.size(), descriptorSets.data(), 0, nullptr);

        renderer->Render(commandBuffer);
    }
}
