//
// Created by Remus on 9/01/2024.
//

#pragma once
#include "Renderer.h"
#include "Base/Common/Data/Mesh.h"


class MeshRenderer final : public Renderer{
public:
    void LoadMesh(const char *aPath);
    void DestroyRenderer() override;
    void BindRenderer(GraphicsPipeline &aBoundGraphicsPipeline) override;
    void Render(VkCommandBuffer aCommandBuffer, const Scene &aScene) override;

    // todo: allocate these in a pool
    Transform* mTransform;
    Mesh *mMesh;
};