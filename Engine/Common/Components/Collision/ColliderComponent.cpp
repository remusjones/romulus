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
#include "Objects/Entity.h"
#include "Physics/PhysicsSystem.h"

void ColliderComponent::Create(PhysicsSystem* aPhysicsSystem, ColliderCreateInfo& aCreateInfo) {
    m_physicsSystem = aPhysicsSystem;
    btVector3 localInertia;

    if (aCreateInfo.mass > 0)
        aCreateInfo.collisionShape->calculateLocalInertia(aCreateInfo.mass, localInertia);

    auto* motionState = new btDefaultMotionState();
    const btRigidBody::btRigidBodyConstructionInfo cInfo(aCreateInfo.mass, motionState, aCreateInfo.collisionShape,
                                                         localInertia);
    m_rigidBody = new btRigidBody(cInfo);
    m_rigidBody->setUserIndex(-1);
    m_rigidBody->setSleepingThresholds(aCreateInfo.linearSleepThreshold, aCreateInfo.angularSleepThreshold);
    m_rigidBody->setFriction(aCreateInfo.friction);
    m_rigidBody->setRollingFriction(aCreateInfo.rollingFriction);
    m_rigidBody->setSpinningFriction(aCreateInfo.spinningFriction);

    aPhysicsSystem->AddRigidBody(m_rigidBody);
}

void ColliderComponent::Tick(const float aDeltaTime) {
    Component::Tick(aDeltaTime);

    if (m_rigidBody) {

        // Test to see whether matrix was modified externally since last frame
        // TODO: investigate on how to only wake up nearby colliders
        if (m_matrixLastFrame != m_attachedEntity->transform.GetWorldMatrix()) {
            m_rigidBody->proceedToTransform(CollisionHelper::TransformToBulletTransform(m_attachedEntity->transform));
            m_rigidBody->clearForces();
            m_rigidBody->clearGravity();

            m_physicsSystem->AwakeRigidBodies();
        }

        glm::mat4 rigidBodyMatrix;
        m_rigidBody->getWorldTransform().getOpenGLMatrix(value_ptr(rigidBodyMatrix));

        m_attachedEntity->transform.SetWorldPosition(CollisionHelper::BulletToGlm(
                m_rigidBody->getWorldTransform().getOrigin()));

        m_attachedEntity->transform.SetWorldRotation(CollisionHelper::BulletToGlm(
                m_rigidBody->getWorldTransform().getRotation()));

        m_matrixLastFrame = m_attachedEntity->transform.GetWorldMatrix();
    }
}

void ColliderComponent::Initialize() {
    Component::Initialize();
    m_rigidBody->setWorldTransform(CollisionHelper::TransformToBulletTransform(m_attachedEntity->transform));
    m_matrixLastFrame = m_attachedEntity->transform.GetWorldMatrix();
}
