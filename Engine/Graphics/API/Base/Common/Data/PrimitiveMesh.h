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
	[[nodiscard]] std::vector<Index> GetIndices() const { return indices; }

	[[nodiscard]] size_t GetVerticesSize() const { return vertices.size(); }
	[[nodiscard]] size_t GetIndicesSize() const { return indices.size(); }

private:
	std::vector<Vertex> vertices;
	std::vector<Index> indices;
	AllocatedBuffer* verticesBuffer = nullptr;
	AllocatedBuffer* indicesBuffer  = nullptr;
};
