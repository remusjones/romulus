//
// Created by Remus on 9/01/2024.
//

#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

#include "GraphicsPipelineFactory.h"


class Cubemap;

class LightingRenderSystem : public GraphicsPipelineFactory {
public:
    LightingRenderSystem(const std::vector<VkDescriptorSetLayout> &aDescriptorLayouts);
protected:
    void CreatePipelineLayout() override;
    void CreatePipeline() override;

};
