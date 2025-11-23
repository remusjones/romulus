//
// Created by Remus on 9/01/2024.
//

#pragma once
#include "vulkan/vulkan_core.h"
#include "Base/Common/Buffers/PushConstants.h"

class GraphicsPipeline;
class Material;

// todo: remove virtual class and make crtp class for reduced virtual lookups
class Renderer
{
public:
    Renderer() = default;
    virtual ~Renderer() = default;

    virtual void BindRenderer(GraphicsPipeline& boundGraphicsPipeline);
    virtual void Render(VkCommandBuffer commandBuffer);
    virtual void DestroyRenderer();


    void SetMaterial(uint8_t index, Material* inMaterial) { material = inMaterial; }
    [[nodiscard]] Material* GetMaterial(uint8_t index) const { return material; }
    [[nodiscard]] GraphicsPipeline* GetGraphicsPipeline() const { return graphicsPipeline; }

protected:
    GraphicsPipeline* graphicsPipeline{};
    Material* material{};
    PushConstants pushConstants{};
};
