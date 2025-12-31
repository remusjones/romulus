//
// Created by Remus on 20/12/2023.
//

#pragma once
#include <vector>
#include "Base/Common/Data/Vertex.h"
#include "AllocatedBuffer.h"
#include "EASTL/unique_ptr.h"

class AllocatedVertexBuffer
{
public:
	AllocatedVertexBuffer() = default;
	AllocatedVertexBuffer(const eastl::vector<Vertex>& vertices, const eastl::vector<Index>& indices);

	void Destroy();

public:
	// todo: replace external usages of these with helper functions
	eastl::unique_ptr<AllocatedBuffer> verticesBuffer;
	eastl::unique_ptr<AllocatedBuffer> indicesBuffer;
};
