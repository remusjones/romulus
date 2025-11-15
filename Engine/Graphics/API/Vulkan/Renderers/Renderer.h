//
// Created by Remus on 9/01/2024.
//

#pragma once
#include "Base/Common/Buffers/PushConstants.h"
#include "Scenes/Scene.h"

class Material;

class Renderer
{
public:
    Renderer() = default;

    virtual ~Renderer() = default;
    virtual void Render(VkCommandBuffer commandBuffer, const Scene& scene);
    virtual void BindRenderer(GraphicsPipeline& boundGraphicsPipeline);
    virtual void DestroyRenderer();

    GraphicsPipeline* graphicsPipeline;
    Material* material;
    PushConstants pushConstants{};
};
