//
// Created by Remus on 22/12/2023.
//

#pragma once

#include <memory>

#include "Base/Common/Buffers/AllocatedBuffer.h"
#include "Base/Common/Data/GPUSceneData.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"
#include "Objects/FlyCamera.h"
#include "Objects/ImGuiDebugLayer.h"

class GraphicsPipelineFactory;
class Texture;
class btRigidBody;
class btVector3;
class Material;
class PhysicsSystem;
class GraphicsPipeline;
class Camera;
class MeshObject;

class Scene : public ImGuiDebugLayer
{
public:
	virtual ~Scene();

	virtual void PreConstruct(const char* aSceneName);
	virtual void Construct();
	virtual void Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
	                    uint32_t aCurrentFrame);


	virtual void TickPhysics(float deltaTime);
	virtual void Tick(float aDeltaTime);
	virtual void Cleanup();
	void OnImGuiRender() override;

	void AddRenderPipeline(GraphicsPipelineFactory* inPipelineFactory);


	MeshObject* CreateObject(const eastl::string_view& inName,
	                         const eastl::string_view& inPath,
	                         Material& inMaterial,
	                         GraphicsPipeline& inPipeline,
	                         const glm::vec3& aPos = glm::vec3(0),
	                         const glm::vec3& aRot = glm::vec3(0),
	                         const glm::vec3& aScale = glm::vec3(1)
	);

	// todo: inconsistent pointer semantics here with & or *

	template<typename T, typename ...Args>
	T* MakeEntity(Args&&... args)
	{
		T* object = static_cast<T*>(sceneObjects.emplace_back(eastl::make_unique<T>(eastl::forward<Args>(args)...)).get());
		sceneTransformRelationships.insert_or_assign(&object->transform, object);
		return object;
	}


	void AttachSphereCollider(SceneObject& aEntity, const float radius, const float mass, float friction = 0.5f) const;
	void AttachBoxCollider(SceneObject& inEntity, glm::vec3 halfExtents, float mass, float friction = 0.5f) const;

	// TODO: probably bind these to flycam instead?
	void MouseMovement(const SDL_MouseMotionEvent& aMouseMotion);

	void MouseInput(const SDL_MouseButtonEvent& aMouseInput);

	const btRigidBody* PickRigidBody(int x, int y) const;

	Texture* CreateTexture(const char* aName, const eastl::vector<eastl::string>& aPathsSet);

protected:
	virtual SceneObject& MakeEntity(); // todo: move to factory
private:
	void DrawObjectsRecursive(SceneObject& entityToDraw);
	bool IsParentOfPickedEntity(const SceneObject& obj);
	void ChangeImGuizmoOperation(int aOperation);

public:
	// TODO: Make unique ptr
	FlyCamera* activeCamera;
	std::vector<GraphicsPipelineFactory*> renderPipelines;
	GPUSceneData sceneData;
	const char* m_sceneName; //
	PhysicsSystem* physicsSystem;

protected:
	// todo: feels like these probably shouldn't be here
	eastl::vector<eastl::unique_ptr<SceneObject>> sceneObjects;

	eastl::hash_map<Transform*, SceneObject*> sceneTransformRelationships;
	eastl::hash_map<eastl::string, eastl::unique_ptr<Texture>> sceneTextures;

private:
	// TODO: Make unique ptr
	// todo: we don't really have a need for this!
	PhysicsSystem* sceneInteractionPhysicsSystem = nullptr;
	SceneObject* pickedEntity{nullptr};
	int mouseX{0}, mouseY{0};
};
