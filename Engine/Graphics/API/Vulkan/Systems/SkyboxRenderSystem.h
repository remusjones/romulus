//
// Created by Remus on 7/01/2024.
//

#pragma once
#include "GraphicsPipelineFactory.h"


class Cubemap;

class SkyboxRenderSystem : public GraphicsPipelineFactory {
protected:
    void CreatePipelineLayout() override;
    void CreatePipeline() override;
};
