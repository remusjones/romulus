//
// Created by Remus on 22/12/2023.
//

#pragma once

#include <memory>

#include "Base/Common/Buffers/AllocatedBuffer.h"
#include "Base/Common/Data/GPUSceneData.h"
#include "Objects/FlyCamera.h"
#include "Objects/ImGuiLayer.h"

class RenderSystemBase;
class Texture;
class btRigidBody;
class btVector3;
class Material;
class PhysicsSystem;
class GraphicsPipeline;
class Camera;
class MeshObject;

class Scene : public ImGuiLayer {
public:
    virtual ~Scene() = default;

    virtual void PreConstruct(const char *aSceneName);


    virtual void Construct();

    virtual void Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                        uint32_t aCurrentFrame);


    virtual void Tick(float aDeltaTime);

    virtual void Cleanup();

    void OnImGuiRender() override;

    void AddRenderSystem(RenderSystemBase * aRenderSystem);


    MeshObject *CreateObject(const char *aName,
                           const char *aMeshPath,
                           Material &aMaterial,
                           GraphicsPipeline &aPipeline,
                           glm::vec3 aPos = glm::vec3(0),
                           glm::vec3 aRot = glm::vec3(0),
                           glm::vec3 aScale = glm::vec3(1)
    );

    void AddEntity(std::unique_ptr<Entity> aEntity);
    void AddEntity(Entity* aEntity);

    void AttachSphereCollider(Entity &aEntity, const float aRadius, const float aMass, float aFriction = 0.5f) const;

    void AttachBoxCollider(Entity &aEntity, glm::vec3 aHalfExtents, float aMass, float aFriction = 0.5f) const;

    // TODO: probably bind these to flycam instead?
    void MouseMovement(const SDL_MouseMotionEvent &aMouseMotion);

    void MouseInput(const SDL_MouseButtonEvent &aMouseInput);

    const btRigidBody *PickRigidBody(int x, int y) const;

    Texture* CreateTexture(const char *aName, std::vector<std::string> aPathsSet);
protected:
    virtual Entity* MakeEntity(); // todo: move to factory
private:
    void DrawObjectsRecursive(Entity& entityToDraw);
    bool IsParentOfPickedEntity(const Entity& obj);
    void ChangeImGuizmoOperation(int aOperation);

public:
    // TODO: Make unique ptr
    FlyCamera* activeCamera;
    std::vector<RenderSystemBase*> mRenderSystems;
    GPUSceneData sceneData;
    const char* m_sceneName; //
    PhysicsSystem* m_physicsSystem;

protected:
    // TODO: Make unique ptr
    std::vector<std::unique_ptr<Entity>> mObjects;
    std::unordered_map<Transform*, Entity*> mTransformEntityRelationships;
    std::unordered_map<std::string, std::unique_ptr<Texture>> mLoadedTextures;
private:
    // TODO: Make unique ptr
    PhysicsSystem* m_SceneInteractionPhysicsSystem;
    Entity* m_PickedEntity{nullptr};
    int m_mouseX{0}, m_mouseY{0};


};
