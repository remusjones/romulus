//
// Created by Remus on 20/12/2023.
//

#pragma once
#include <vector>
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
	void CalculateTangents(std::vector<Vertex>& vertices, const std::vector<int32_t>& indices);

	[[nodiscard]] std::vector<Vertex> GetVertices() const { return vertices; }
	[[nodiscard]] std::vector<int32_t> GetIndices() const { return indices; }

	[[nodiscard]] uint32_t GetVerticesSize() const { return vertices.size(); }
	[[nodiscard]] uint32_t GetIndicesSize() const { return indices.size(); }

private:
	std::vector<Vertex> vertices;
	std::vector<int32_t> indices;

	eastl::unique_ptr<AllocatedVertexBuffer> vertexBuffer;
};
