//
// Created by Remus on 15/01/2024.
//

#include "CollisionHelper.h"

#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btTriangleMesh.h"


btTransform CollisionHelper::TransformToBulletTransform(const Transform& otherTransform)
{
	return btTransform(GlmToBullet(otherTransform.GetLocalRotation()), GlmToBullet(otherTransform.GetLocalPosition()));
}


btVector3 CollisionHelper::GlmToBullet(const glm::vec3& otherVector)
{
	return btVector3(otherVector.x, otherVector.y, otherVector.z);
}

btQuaternion CollisionHelper::GlmToBullet(const glm::quat& otherQuaternion)
{
	return btQuaternion(otherQuaternion.x, otherQuaternion.y, otherQuaternion.z, otherQuaternion.w);
}

glm::vec3 CollisionHelper::BulletToGlm(const btVector3& otherVector)
{
	return glm::vec3(otherVector.getX(), otherVector.getY(), otherVector.getZ());
}

glm::quat CollisionHelper::BulletToGlm(const btQuaternion& otherQuaternion)
{
	return glm::quat(otherQuaternion.w(), otherQuaternion.x(), otherQuaternion.y(), otherQuaternion.z());
}

btBvhTriangleMeshShape* CollisionHelper::MakeCollisionMesh(const std::vector<Vertex>& inVertices,
                                                           const std::vector<int32_t>& inIndices)
{
	btTriangleMesh* triangleMesh = new btTriangleMesh(); // Bullet Physics triangle mesh

	for (int i = 0; i < inIndices.size(); i += 3)
	{
		btVector3 vertex0(inVertices[inIndices[i]].position.x, inVertices[inIndices[i]].position.x,
		                  inVertices[inIndices[i]].position.z);
		btVector3 vertex1(inVertices[inIndices[i + 1]].position.x, inVertices[inIndices[i + 1]].position.y,
		                  inVertices[inIndices[i + 1]].position.z);
		btVector3 vertex2(inVertices[inIndices[i + 2]].position.x, inVertices[inIndices[i + 2]].position.y,
		                  inVertices[inIndices[i + 2]].position.z);
		triangleMesh->addTriangle(vertex0, vertex1, vertex2);
	}

	return new btBvhTriangleMeshShape(triangleMesh, true);
}

btBoxShape* CollisionHelper::MakeAABBCollision(const std::vector<Vertex>& inVertices)
{
	// Find min and max vertices along each axis
	btVector3 minVertex(inVertices[0].position.x, inVertices[0].position.y, inVertices[0].position.z);
	btVector3 maxVertex(inVertices[0].position.x, inVertices[0].position.y, inVertices[0].position.z);

	for (const Vertex& vertex : inVertices)
	{
		minVertex.setX(std::min(minVertex.x(), vertex.position.x));
		minVertex.setY(std::min(minVertex.y(), vertex.position.y));
		minVertex.setZ(std::min(minVertex.z(), vertex.position.z));

		maxVertex.setX(std::max(maxVertex.x(), vertex.position.x));
		maxVertex.setY(std::max(maxVertex.y(), vertex.position.y));
		maxVertex.setZ(std::max(maxVertex.z(), vertex.position.z));
	}

	// Calculate extents and half extents for the box shape
	const btVector3 extents     = maxVertex - minVertex;
	const btVector3 halfExtents = extents * 0.5f;

	// Create and return AABB (Axis-Aligned Bounding Box)
	return new btBoxShape(halfExtents);
}
