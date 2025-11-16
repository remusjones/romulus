//
// Created by Remus on 17/12/2023.
//

#pragma once

#include "Objects/SceneObject.h"
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

// todo the rendering here should just be a component ..
class MeshObject : public SceneObject
{
public:
    void Construct() override;
    void Tick(float deltaTime) override;
    void Cleanup() override;
    void OnImGuiRender() override;
    void CreateObject(Material& aMaterial, const eastl::string_view& objectName);

    MeshRenderer& GetRenderer() {return meshRenderer; }

private:
    MeshRenderer meshRenderer;
};
