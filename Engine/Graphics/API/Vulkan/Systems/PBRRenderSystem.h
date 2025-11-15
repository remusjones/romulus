//
// Created by Remus on 9/01/2024.
//

#pragma once
#include "GraphicsPipelineFactory.h"


class PBRRenderSystem : public GraphicsPipelineFactory{
    void CreatePipelineLayout() override;
    void CreatePipeline() override;
};
