//
// Created by Remus on 17/12/2023.
//

#pragma once

#include "Objects/Entity.h"
#include "Vulkan/Renderers/MeshRenderer.h"
#include "Vulkan/Renderers/Renderer.h"

class Scene;
class Mesh;
class Material;
class RomulusVulkanRenderer;
class GraphicsPipeline;
class Buffer;
class CameraInformationBuffer;
class AllocatedVertexBuffer;


/*Attempts to abstract the required components for rendering to
 * identify what can be seperated from render pipeline */
class MeshObject : public Entity{
public:
    void Construct() override;
    void Tick(float deltaTime) override;
    void Cleanup() override;
    void OnImGuiRender() override;
    void CreateObject(Material &aMaterial, const char *aName = "Default");

    MeshRenderer meshRenderer;
};
