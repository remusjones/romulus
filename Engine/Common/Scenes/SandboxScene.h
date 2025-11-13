//
// Created by Remus on 22/12/2023.
//

#pragma once

#include <memory>
#include "Scene.h"
#include "Vulkan/Common/MaterialFactory.h"
#include "Vulkan/Systems/LineRenderSystem.h"
#include "Vulkan/Systems/PBRRenderSystem.h"
#include "Vulkan/Systems/PipelineConfigInfo.h"
#include "Vulkan/Systems/SkyboxRenderSystem.h"
#include "Vulkan/Systems/UnlitRenderSystem.h"
#include "Vulkan/Systems/WireframeRenderSystem.h"

class LineRenderer;
class Cubemap;
class Primitive;
class Texture;

class SandboxScene final : public Scene
{
public:
    void PreConstruct(const char* inSceneName) override;

    void Construct() override;

    void Tick(float deltaTime) override;

    void Cleanup() override;

    void OnImGuiRender() override;

private:
    PipelineConfigInfo defaultPipelineConfig;

    VkSampler blockySampler;

    // TODO: Remove unmanaged pointer usage
    MeshObject* monkey;
    MeshObject* teapot;
    MeshObject* light;
    MeshObject* floor;
    LineRenderer* lineRenderer;
    Primitive* cubeMapMesh;
    Primitive* lineRendererEntity;

    Cubemap* cubemap;
    std::unique_ptr<SkyboxRenderSystem> cubemapRenderPipeline;
    std::unique_ptr<UnlitRenderSystem> unlitRenderPipeline;
    std::unique_ptr<WireframeRenderSystem> wireframeRenderPipeline;
    std::unique_ptr<PBRRenderSystem> pbrRenderPipeline;
    std::unique_ptr<LineRenderSystem> lineRendererPipeline;

    MaterialFactory mMaterialUnlitFactory;
    MaterialFactory mMaterialPBRFactory;
    MaterialFactory mGenericMaterialFactory;
};
