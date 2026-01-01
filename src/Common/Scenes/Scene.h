//
// Created by Remus on 22/12/2023.
//

#pragma once

#include <memory>

#include "Base/Common/MeshAllocator.h"
#include "Base/Common/Buffers/AllocatedBuffer.h"
#include "Base/Common/Data/GPUSceneData.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"
#include "Objects/FlyCamera.h"

class IDebugRegistry;
class GraphicsPipelineFactory;
class Texture;
class btRigidBody;
class btVector3;
class Material;
class PhysicsSystem;
class GraphicsPipeline;
class Camera;
class MeshObject;

class Scene : public IDebuggable
{
public:
	virtual ~Scene();
	explicit Scene(IDebugRegistry* inDebugRegistry);

	virtual void PreConstruct(const char* aSceneName);
	virtual void Construct();
	virtual void Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
	                    uint32_t aCurrentFrame);


	virtual void TickPhysics(float deltaTime);
	virtual void Tick(float deltaTime);
	virtual void Destroy();
	void OnDebugGui() override;

	void AddRenderPipeline(GraphicsPipelineFactory* inPipelineFactory);


	MeshObject* CreateObject(const eastl::string_view& inName,
	                         const eastl::string_view& inPath,
	                         Material& inMaterial,
	                         GraphicsPipeline& inPipeline,
	                         const glm::vec3& inPos = glm::vec3(0),
	                         const glm::vec3& inRot = glm::vec3(0),
	                         const glm::vec3& inScale = glm::vec3(1)
	);

	template<typename T, typename ...Args>
	T* MakeEntity(Args&&... args)
	{
		SceneObjectId newId = ++currentObjectId;

		auto newObjectPtr = eastl::make_unique<T>(eastl::forward<Args>(args)...);
		T* object = newObjectPtr.get();
		object->SetId(newId);

		idToIndexMap.insert_or_assign(newId, sceneObjects.size());
		sceneObjects.push_back(eastl::move(newObjectPtr));
		sceneTransformRelationships.insert_or_assign(&object->transform, object);

		return object;
	}

	SceneObject* GetSceneObject(const SceneObjectId id) { return sceneObjects[idToIndexMap.at(id)].get();}

	void AttachSphereCollider(SceneObject& aEntity, const float radius, const float mass, float friction = 0.5f) const;
	void AttachBoxCollider(SceneObject& inEntity, glm::vec3 halfExtents, float mass, float friction = 0.5f) const;

	// TODO: probably bind these to flycam instead?
	void MouseMovement(const SDL_MouseMotionEvent& aMouseMotion);

	void MouseInput(const SDL_MouseButtonEvent& aMouseInput);

	const btRigidBody* PickRigidBody(int x, int y) const;

	Texture* CreateTexture(const eastl::string_view& aName, const eastl::vector<eastl::string>& aPathsSet);

private:
	void DrawObjectsRecursive(SceneObject& entityToDraw);
	bool IsParentOfPickedEntity(const SceneObject& obj);
	void ChangeImGuizmoOperation(int aOperation);

public:
	eastl::vector<GraphicsPipelineFactory*> renderPipelines;
	GPUSceneData sceneData;

	eastl::unique_ptr<PhysicsSystem> physicsSystem;
	eastl::unique_ptr<FlyCamera> activeCamera;

	// TODO: Make unique ptr
	eastl::string_view m_sceneName;

protected:
	eastl::vector<eastl::unique_ptr<SceneObject>> sceneObjects;
	eastl::hash_map<SceneObjectId, size_t> idToIndexMap;

	eastl::hash_map<Transform*, SceneObject*> sceneTransformRelationships;
	eastl::hash_map<eastl::string_view, eastl::unique_ptr<Texture>> sceneTextures;
	eastl::unique_ptr<MeshAllocator> meshAllocator;

private:
	IDebugRegistry* debugRegistry;
	// TODO: Make unique ptr
	// todo: we don't really have a need for this!
	eastl::unique_ptr<PhysicsSystem> sceneInteractionPhysicsSystem;
	SceneObject* pickedEntity{nullptr};
	int mouseX{0}, mouseY{0};

	SceneObjectId currentObjectId = 0;
};
