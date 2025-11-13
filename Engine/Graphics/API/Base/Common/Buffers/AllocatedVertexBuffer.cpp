//
// Created by Remus on 20/12/2023.
//

#include "AllocatedVertexBuffer.h"

#include "AllocatedBuffer.h"
#include "Base/Common/Data/Vertex.h"

AllocatedVertexBuffer::AllocatedVertexBuffer(const std::vector<Vertex>& vertices,
                                             const std::vector<int32_t>& indices)
{
	verticesBuffer = new AllocatedBuffer(vertices.data(),
	                                      sizeof(vertices[0]) *
	                                      vertices.size(),
	                                      VK_BUFFER_USAGE_TRANSFER_DST_BIT |
	                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

	indicesBuffer = new AllocatedBuffer(indices.data(),
	                                     sizeof(indices[0]) * indices.size(),
	                                     VK_BUFFER_USAGE_TRANSFER_DST_BIT |
	                                     VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

AllocatedVertexBuffer::~AllocatedVertexBuffer()
{
	verticesBuffer->Destroy();
	delete verticesBuffer;
	indicesBuffer->Destroy();
	delete indicesBuffer;
}
