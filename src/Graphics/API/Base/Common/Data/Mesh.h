//
// Created by Remus on 20/12/2023.
//

#pragma once
#include "Vertex.h"
#include "Base/Common/Buffers/AllocatedVertexBuffer.h"
#include "EASTL/unique_ptr.h"

class Mesh
{
public:
	Mesh();

	void Bind(VkCommandBuffer commandBuffer) const;
	void Destroy();
	bool LoadFromObject(const char* fileName);
	void CalculateTangents(const eastl::vector<Vertex>& vertices, const eastl::vector<int32_t>& indices);

	[[nodiscard]] eastl::vector<Vertex> GetVertices() const { return vertices; }
	[[nodiscard]] eastl::vector<int32_t> GetIndices() const { return indices; }

	[[nodiscard]] uint32_t GetVerticesSize() const { return vertices.size(); }
	[[nodiscard]] uint32_t GetIndicesSize() const { return indices.size(); }

private:
	eastl::vector<Vertex> vertices;
	eastl::vector<int32_t> indices;

	eastl::unique_ptr<AllocatedVertexBuffer> vertexBuffer;
};
