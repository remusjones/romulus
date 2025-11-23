//
// Created by Remus on 9/01/2024.
//

#pragma once
#include "Renderer.h"
#include "Math/Transform.h"


class PrimitiveMesh;
class Mesh;

class SkyboxRenderer final : public Renderer
{
public:
	void Render(VkCommandBuffer commandBuffer) override;
	void BindRenderer(GraphicsPipeline& boundGraphicsPipeline) override;
	void DestroyRenderer() override;

	void LoadMesh(const char* path);

	Transform* transform;
	PrimitiveMesh* mesh;
};
