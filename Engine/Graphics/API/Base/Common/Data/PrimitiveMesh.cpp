//
// Created by Remus on 9/01/2024.
//

#include <Base/Common/Data/PrimitiveMesh.h>
#include <LoadUtilities.h>

void PrimitiveMesh::Bind(VkCommandBuffer commandBuffer) const
{
	const VkBuffer vertexBuffers[] = {verticesBuffer->GetBuffer()};
	const VkDeviceSize offsets[]   = {0};

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indicesBuffer->GetBuffer(),
	                     0, VK_INDEX_TYPE_UINT32);
}

bool PrimitiveMesh::LoadFromObject(const char* filename)
{
	if (LoadUtilities::LoadMeshFromDisk(filename, vertices, indices))
	{
		verticesBuffer = new AllocatedBuffer(vertices.data(), (sizeof(vertices[0]) + 1) * vertices.size(),
		                                     VK_BUFFER_USAGE_TRANSFER_DST_BIT |
		                                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

		indicesBuffer = new AllocatedBuffer(indices.data(), (sizeof(indices[0]) + 1) * indices.size(),
		                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT |
		                                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

		vmaSetAllocationName(gGraphics->allocator, verticesBuffer->GetAllocation(),
		                     (std::string(filename) + " mVerticesBuffer").c_str());
		vmaSetAllocationName(gGraphics->allocator, indicesBuffer->GetAllocation(),
		                     (std::string(filename) + " mIndicesBuffer").c_str());
		return true;
	}
	return false;
}

PrimitiveMesh::~PrimitiveMesh()
{
	if (verticesBuffer)
	{
		verticesBuffer->Destroy();
		delete verticesBuffer;
	}

	if (indicesBuffer)
	{
		indicesBuffer->Destroy();
		delete indicesBuffer;
	}
}
