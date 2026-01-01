//
// Created by Remus on 19/01/2024.
//

#pragma once
#include "GraphicsPipelineFactory.h"


class LineRenderSystem : public GraphicsPipelineFactory
{
    void CreatePipelineLayout() override;
    void CreatePipeline() override;
};
