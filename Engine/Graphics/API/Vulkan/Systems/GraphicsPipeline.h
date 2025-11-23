//
// Created by Remus on 17/12/2023.
//

#pragma once
#include <vector>
#include "PipelineConfigInfo.h"
#include "Base/Common/Material.h"

struct PipelineConfigInfo;
class Scene;
class Material;
class MaterialBase;
class Renderer;

class GraphicsPipeline
{
public:
    enum SubPasses : uint8_t
    {
        SUBPASS_GEOMETRY = 0,
        SUBPASS_LIGHTING,
        SUBPASS_TRANSPARENCY,
        NUMBER_OF_SUBPASSES
    };

    GraphicsPipeline(const char* pipelineName, const PipelineConfigInfo& configInfo) : pipelineName(pipelineName),
        romulusPipelineConfig(nullptr),
        depthStencilCreateInfo()
    {
        pipelineConfig = configInfo;
    }

    void CreateShaderModule(const char* path, VkShaderStageFlagBits stage);
    void AddRenderer(Renderer* renderer);
    void Draw(VkCommandBuffer commandBuffer, const Scene& scene) const;
    void Create();
    void Destroy();
    static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

    const char* pipelineName;
    PipelineConfigInfo pipelineConfig;
    VkPipeline romulusPipelineConfig;
    eastl::vector<VkPipelineShaderStageCreateInfo> shaders;
    eastl::vector<Renderer*> renderers;
    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo;
};
