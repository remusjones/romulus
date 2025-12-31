//
// Created by Remus on 13/01/2024.
//

#include "PhysicsSystem.h"

#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <LinearMath/btVector3.h>

void PhysicsSystem::Create()
{
	collisionConfiguration = new btDefaultCollisionConfiguration();
	collisionDispatcher    = new btCollisionDispatcher(collisionConfiguration);
	broadphaseInterface    = new btDbvtBroadphase();
	auto* solver                = new btSequentialImpulseConstraintSolver();
	constraintSolver       = solver;

	dynamicsWorld = new btDiscreteDynamicsWorld(collisionDispatcher, broadphaseInterface, constraintSolver,
	                                              collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, mGravity, 0));
}

void PhysicsSystem::AwakeRigidBodies()
{
	for (int i = 0; i < allocatedRigidbodies.size(); i++)
	{
		allocatedRigidbodies[i]->activate(true);
	}
}

void PhysicsSystem::Tick(const float aDeltaTime)
{
	if (dynamicsWorld)
	{
		dynamicsWorld->stepSimulation(aDeltaTime, 10, 1.f / 240.f);
	}
}

void PhysicsSystem::Destroy()
{
	for (int i = 0; i < allocatedRigidbodies.size(); i++)
	{
		if (dynamicsWorld)
		{
			dynamicsWorld->removeRigidBody(allocatedRigidbodies[i]);
		}
		delete allocatedRigidbodies[i];
	}

	if (dynamicsWorld)
	{
		int i;

		for (i = dynamicsWorld->getNumConstraints() - 1; i >= 0; i--)
		{
			dynamicsWorld->removeConstraint(dynamicsWorld->getConstraint(i));
		}

		for (i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			if (btRigidBody* body = btRigidBody::upcast(obj); body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			dynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}
	}
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		delete collisionShapes[j];
	}


	collisionShapes.clear();

	delete dynamicsWorld;
	dynamicsWorld = nullptr;

	delete constraintSolver;
	constraintSolver = nullptr;

	delete broadphaseInterface;
	broadphaseInterface = nullptr;

	delete collisionDispatcher;
	collisionDispatcher = nullptr;

	delete collisionConfiguration;
	collisionConfiguration = nullptr;
}

btAlignedObjectArray<btRigidBody*> PhysicsSystem::GetRigidBodies() const
{
	return allocatedRigidbodies;
}

btAlignedObjectArray<btCollisionShape*> PhysicsSystem::GetCollisionShapes() const
{
	return collisionShapes;
}

btBroadphaseInterface* PhysicsSystem::GetBroadPhase() const
{
	return broadphaseInterface;
}

btCollisionDispatcher* PhysicsSystem::GetDispatcher() const
{
	return collisionDispatcher;
}

btConstraintSolver* PhysicsSystem::GetConstraintSolver() const
{
	return constraintSolver;
}

btDefaultCollisionConfiguration* PhysicsSystem::GetCollisionConfiguration() const
{
	return collisionConfiguration;
}

btDiscreteDynamicsWorld* PhysicsSystem::GetDynamicsWorld() const
{
	return dynamicsWorld;
}

void PhysicsSystem::AddRigidBody(btRigidBody* aRigidBody)
{
	dynamicsWorld->addRigidBody(aRigidBody);
	allocatedRigidbodies.push_back(aRigidBody);
	collisionShapes.push_back(aRigidBody->getCollisionShape());
}

void PhysicsSystem::AddCollisionShape(btCollisionShape* aCollisionShape)
{
	collisionShapes.push_back(aCollisionShape);
}
