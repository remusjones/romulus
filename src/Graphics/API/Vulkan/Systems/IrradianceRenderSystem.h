//
// Created by Remus on 10/01/2024.
//

#pragma once
#include "GraphicsPipelineFactory.h"

class IrradianceRenderSystem : GraphicsPipelineFactory {
protected:
    void CreatePipelineLayout() override;

    void CreatePipeline() override;
};
