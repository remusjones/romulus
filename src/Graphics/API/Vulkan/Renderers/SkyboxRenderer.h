//
// Created by Remus on 9/01/2024.
//

#pragma once
#include "Renderer.h"
#include "Base/Common/MeshAllocator.h"
#include "Base/Common/Data/Mesh.h"
#include "Math/Transform.h"


class PrimitiveMesh;

class SkyboxRenderer final : public Renderer
{
public:
	void Render(VkCommandBuffer commandBuffer) override;
	void BindRenderer(GraphicsPipeline& boundGraphicsPipeline) override;
	void DestroyRenderer() override;

	void LoadMesh(MeshAllocator& allocator, const char* path);

	Transform* transform;
	Mesh* mesh;
};
