//
// Created by Remus on 9/01/2024.
//

#pragma once
#include <vector>
#include <Base/Common/Data/Vertex.h>


class AllocatedBuffer;

class PrimitiveMesh
{

public:
	void Bind(VkCommandBuffer commandBuffer) const;
	bool LoadFromObject(const char* filename);

	~PrimitiveMesh();

	[[nodiscard]] std::vector<Vertex> GetVertices() const { return vertices; }
	[[nodiscard]] std::vector<int32_t> GetIndices() const { return indices; }

	[[nodiscard]] int32_t GetVerticesSize() const { return vertices.size(); }
	[[nodiscard]] int32_t GetIndicesSize() const { return indices.size(); }

private:
	std::vector<Vertex> vertices;
	std::vector<int32_t> indices;
	AllocatedBuffer* verticesBuffer = nullptr;
	AllocatedBuffer* indicesBuffer  = nullptr;
};
