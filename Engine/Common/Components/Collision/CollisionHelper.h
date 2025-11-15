//
// Created by Remus on 15/01/2024.
//

#pragma once
#include "Base/Common/Data/Vertex.h"
#include "LinearMath/btTransform.h"
#include "Math/Transform.h"


class btBoxShape;
class btBvhTriangleMeshShape;

class CollisionHelper
{
public:
	static btTransform TransformToBulletTransform(const Transform& otherTransform);
	static btVector3 GlmToBullet(const glm::vec3& otherVector);
	static btQuaternion GlmToBullet(const glm::quat& otherQuaternion);
	static glm::vec3 BulletToGlm(const btVector3& otherVector);
	static glm::quat BulletToGlm(const btQuaternion& otherQuaternion);
	static btBvhTriangleMeshShape* MakeCollisionMesh(const std::vector<Vertex>& inVertices, const std::vector<int32_t>&
	                                                 inIndices);
	static btBoxShape* MakeAABBCollision(const std::vector<Vertex>& inVertices);
};
