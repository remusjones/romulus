//
// Created by Remus on 20/12/2023.
//

#include "AllocatedVertexBuffer.h"

#include "AllocatedBuffer.h"
#include "Base/Common/Data/Vertex.h"

AllocatedVertexBuffer::AllocatedVertexBuffer(const eastl::vector<Vertex>& vertices,
                                             const eastl::vector<Index>& indices)
{
	verticesBuffer = eastl::make_unique<AllocatedBuffer>(vertices.data(),
	                                      sizeof(Vertex) * vertices.size(),
	                                      VK_BUFFER_USAGE_TRANSFER_DST_BIT |
	                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

	indicesBuffer = eastl::make_unique<AllocatedBuffer>(indices.data(),
	                                     sizeof(Index) * indices.size(),
	                                     VK_BUFFER_USAGE_TRANSFER_DST_BIT |
	                                     VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

void AllocatedVertexBuffer::Destroy()
{
    if (verticesBuffer)
    {
        verticesBuffer->Destroy();
        verticesBuffer.reset();
    }

    if (indicesBuffer)
    {
        indicesBuffer->Destroy();
        indicesBuffer.reset();
    }
}