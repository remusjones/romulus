#pragma once
#include "LinearMath/btAlignedObjectArray.h"


class btRigidBody;
class btDiscreteDynamicsWorld;
class btDefaultCollisionConfiguration;
class btConstraintSolver;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btCollisionShape;

class PhysicsSystem {
public:

    void Create();
    void AwakeRigidBodies();
    void Tick(float aDeltaTime);
    void Destroy();
    void AddRigidBody(btRigidBody* aRigidBody);
    void AddCollisionShape(btCollisionShape* aCollisionShape);

    [[nodiscard]] btAlignedObjectArray <btRigidBody*> GetRigidBodies() const;
    [[nodiscard]] btAlignedObjectArray <btCollisionShape*> GetCollisionShapes() const;
    [[nodiscard]] btBroadphaseInterface* GetBroadPhase() const;
    [[nodiscard]] btCollisionDispatcher* GetDispatcher() const;
    [[nodiscard]] btConstraintSolver* GetConstraintSolver() const;
    [[nodiscard]] btDefaultCollisionConfiguration* GetCollisionConfiguration() const;
    [[nodiscard]] btDiscreteDynamicsWorld* GetDynamicsWorld() const;


private:
    btAlignedObjectArray<btCollisionShape*> collisionShapes;
    btAlignedObjectArray<btRigidBody*> allocatedRigidbodies;

    btBroadphaseInterface* broadphaseInterface;
    btCollisionDispatcher* collisionDispatcher;
    btConstraintSolver* constraintSolver;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btDiscreteDynamicsWorld* dynamicsWorld;

    float_t mGravity = -9.81f;
};