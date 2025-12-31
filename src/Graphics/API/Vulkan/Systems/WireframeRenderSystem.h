//
// Created by Remus on 25/01/2024.
//

#pragma once
#include "GraphicsPipelineFactory.h"


class WireframeRenderSystem : public GraphicsPipelineFactory{
    void CreatePipelineLayout() override;
    void CreatePipeline() override;
};
