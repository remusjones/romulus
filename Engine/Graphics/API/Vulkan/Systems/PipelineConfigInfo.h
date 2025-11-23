//
// Created by Remus on 7/01/2024.
//

#pragma once
#include <vulkan/vulkan.h>
#include <EASTL/vector.h>

struct PipelineConfigInfo {

    // input assembly stage
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

    eastl::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;

    eastl::vector<VkVertexInputBindingDescription> mBindingDescriptions{};
    eastl::vector<VkVertexInputAttributeDescription> mAttributeDescriptions{};
};