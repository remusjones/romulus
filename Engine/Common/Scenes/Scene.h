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
#include "Objects/ImGuiLayer.h"

class GraphicsPipelineFactory;
class Texture;
class btRigidBody;
class btVector3;
class Material;
class PhysicsSystem;
class GraphicsPipeline;
class Camera;
class MeshObject;

class Scene : public ImGuiLayer
{
public:
	virtual ~Scene();

	virtual void PreConstruct(const char* aSceneName);
	virtual void Construct();
	virtual void Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
	                    uint32_t aCurrentFrame);

	virtual void Tick(float aDeltaTime);
	virtual void Cleanup();
	void OnImGuiRender() override;

	void AddRenderPipeline(GraphicsPipelineFactory* inPipelineFactory);


	MeshObject* CreateObject(const char* aName,
	                         const char* aMeshPath,
	                         Material& inMaterial,
	                         GraphicsPipeline& inPipeline,
	                         const glm::vec3& aPos = glm::vec3(0),
	                         const glm::vec3& aRot = glm::vec3(0),
	                         const glm::vec3& aScale = glm::vec3(1)
	);

	// todo: inconsistent pointer semantics here with & or *
	void AddEntity(std::unique_ptr<Entity> aEntity);
	void AddEntity(Entity* aEntity);

	void AttachSphereCollider(Entity& aEntity, const float radius, const float mass, float friction = 0.5f) const;
	void AttachBoxCollider(Entity& inEntity, glm::vec3 halfExtents, float mass, float friction = 0.5f) const;

	// TODO: probably bind these to flycam instead?
	void MouseMovement(const SDL_MouseMotionEvent& aMouseMotion);

	void MouseInput(const SDL_MouseButtonEvent& aMouseInput);

	const btRigidBody* PickRigidBody(int x, int y) const;

	Texture* CreateTexture(const char* aName, const eastl::vector<eastl::string>& aPathsSet);

protected:
	virtual Entity* MakeEntity(); // todo: move to factory
private:
	void DrawObjectsRecursive(Entity& entityToDraw);
	bool IsParentOfPickedEntity(const Entity& obj);
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
	eastl::vector<std::unique_ptr<Entity>> sceneObjects;
	eastl::hash_map<Transform*, Entity*> sceneTransformRelationships;
	eastl::hash_map<eastl::string, std::unique_ptr<Texture>> sceneTextures;

private:
	// TODO: Make unique ptr
	// todo: we don't really have a need for this!
	PhysicsSystem* sceneInteractionPhysicsSystem = nullptr;
	Entity* pickedEntity{nullptr};
	int mouseX{0}, mouseY{0};
};
