//
// Created by Remus on 9/01/2024.
//

#pragma once
#include "Renderer.h"
#include "Base/Common/Data/Mesh.h"
#include "Math/Transform.h"


class MeshRenderer final : public Renderer
{
public:
    void LoadMesh(const char* aPath);
    void DestroyRenderer() override;
    void BindRenderer(GraphicsPipeline& aBoundGraphicsPipeline) override;
    void Render(VkCommandBuffer commandBuffer) override;

    void SetTransform(Transform* inTransform) {mTransform = inTransform; }
    [[nodiscard]] Transform* GetTransform() const { return mTransform; }

protected:
    // todo: allocate these in a pool
    Transform* mTransform = nullptr;
    Mesh* mMesh = nullptr;
};
