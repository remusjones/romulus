//
// Created by Remus on 13/01/2024.
//

#include "ColliderComponent.h"

#include "CollisionHelper.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "glm/gtc/type_ptr.hpp"
#include "LinearMath/btDefaultMotionState.h"
#include "Objects/SceneObject.h"
#include "Physics/PhysicsSystem.h"

void ColliderComponent::Create(PhysicsSystem* inPhysicsSystem, const ColliderCreateInfo& inColliderInfo)
{
	physicsSystem = inPhysicsSystem;
	btVector3 localInertia;

	if (inColliderInfo.mass > 0)
	{
		inColliderInfo.collisionShape->calculateLocalInertia(inColliderInfo.mass, localInertia);
	}

	auto* motionState = new btDefaultMotionState();
	const btRigidBody::btRigidBodyConstructionInfo cInfo(inColliderInfo.mass, motionState, inColliderInfo.collisionShape,
	                                                     localInertia);
	rigidBody = new btRigidBody(cInfo);
	rigidBody->setUserIndex(-1);
	rigidBody->setSleepingThresholds(inColliderInfo.linearSleepThreshold, inColliderInfo.angularSleepThreshold);
	rigidBody->setFriction(inColliderInfo.friction);
	rigidBody->setRollingFriction(inColliderInfo.rollingFriction);
	rigidBody->setSpinningFriction(inColliderInfo.spinningFriction);

	inPhysicsSystem->AddRigidBody(rigidBody);
}

void ColliderComponent::Tick(const float aDeltaTime)
{
	Component::Tick(aDeltaTime);

	if (rigidBody)
	{
		// Test to see whether matrix was modified externally since last frame
		// TODO: investigate on how to only wake up nearby colliders
		if (matrixLastFrame != attachedEntity->transform.GetWorldMatrix())
		{
			rigidBody->proceedToTransform(CollisionHelper::TransformToBulletTransform(attachedEntity->transform));
			rigidBody->clearForces();
			rigidBody->clearGravity();

			physicsSystem->AwakeRigidBodies();
		}

		glm::mat4 rigidBodyMatrix;
		rigidBody->getWorldTransform().getOpenGLMatrix(value_ptr(rigidBodyMatrix));

		attachedEntity->transform.SetWorldPosition(CollisionHelper::BulletToGlm(
			rigidBody->getWorldTransform().getOrigin()));

		attachedEntity->transform.SetWorldRotation(CollisionHelper::BulletToGlm(
			rigidBody->getWorldTransform().getRotation()));

		matrixLastFrame = attachedEntity->transform.GetWorldMatrix();
	}
}

void ColliderComponent::Initialize()
{
	Component::Initialize();
	rigidBody->setWorldTransform(CollisionHelper::TransformToBulletTransform(attachedEntity->transform));
	matrixLastFrame = attachedEntity->transform.GetWorldMatrix();
}
