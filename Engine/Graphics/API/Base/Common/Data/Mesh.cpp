//
// Created by Remus on 20/12/2023.
//

#include "Mesh.h"

#include "LoadUtilities.h"
#include "Base/Common/Buffers/AllocatedBuffer.h"
#include "Base/Common/Buffers/AllocatedVertexBuffer.h"

Mesh::Mesh() : vertexBuffer(nullptr)
{
}

Mesh::~Mesh()
{
	delete vertexBuffer;
}

void Mesh::Bind(VkCommandBuffer commandBuffer) const
{
	const VkBuffer vertexBuffers[] = {vertexBuffer->mVerticesBuffer->GetBuffer()};
	const VkDeviceSize offsets[]   = {0};

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, vertexBuffer->mIndicesBuffer->GetBuffer(),
	                     0, VK_INDEX_TYPE_UINT32);
}

bool Mesh::LoadFromObject(const char* fileName)
{
	if (LoadUtilities::LoadMeshFromDisk(fileName, vertices, indices))
	{
		CalculateTangents(vertices, indices);

		vertexBuffer = new AllocatedVertexBuffer(vertices, indices);

		std::string bufferName;
		bufferName.append(fileName);
		vmaSetAllocationName(gGraphics->allocator, vertexBuffer->mVerticesBuffer->GetAllocation(),
		                     (bufferName + " mVerticesBuffer").c_str());

		vmaSetAllocationName(gGraphics->allocator, vertexBuffer->mIndicesBuffer->GetAllocation(),
		                     (bufferName + " mIndicesBuffer").c_str());
		return true;
	}
	return false;
}

void Mesh::CalculateTangents(std::vector<Vertex>& vertices, const std::vector<int32_t>& indices)
{
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		const int index0 = indices[i];
		const int index1 = indices[i + 1];
		const int index2 = indices[i + 2];

		glm::vec3& v0 = vertices[index0].mPosition;
		glm::vec3& v1 = vertices[index1].mPosition;
		glm::vec3& v2 = vertices[index2].mPosition;

		glm::vec2& uv0 = vertices[index0].mUV;
		glm::vec2& uv1 = vertices[index1].mUV;
		glm::vec2& uv2 = vertices[index2].mUV;

		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		const glm::vec2 deltaUV1 = uv1 - uv0;
		const glm::vec2 deltaUV2 = uv2 - uv0;

		const float r             = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		const glm::vec3 tangent   = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		vertices[index0].mTangent = glm::vec4(tangent.x, tangent.y, tangent.z, 0);
		vertices[index1].mTangent = glm::vec4(tangent.x, tangent.y, tangent.z, 0);
		vertices[index2].mTangent = glm::vec4(tangent.x, tangent.y, tangent.z, 0);
	}
}
