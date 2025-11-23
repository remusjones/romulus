//
// Created by Remus on 20/12/2023.
//

#pragma once
#include <vector>
#include "Base/Common/Data/Vertex.h"
#include "AllocatedBuffer.h"

class AllocatedVertexBuffer
{
public:
	AllocatedVertexBuffer() = default;
	AllocatedVertexBuffer(const std::vector<Vertex>& vertices, const std::vector<Index>& indices);


	~AllocatedVertexBuffer();

	AllocatedBuffer* verticesBuffer;
	AllocatedBuffer* indicesBuffer;
};
