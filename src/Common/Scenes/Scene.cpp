//
// Created by Remus on 22/12/2023.
//

#include "Scene.h"

#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "ImGuizmo.h"

#include <FileManagement.h>
#include <VulkanGraphicsImpl.h>

#include <Base/Common/Buffers/Texture.h>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <Components/Collision/ColliderComponent.h>
#include <Components/Collision/CollisionHelper.h>
#include <glm/gtx/string_cast.hpp>
#include <Physics/PhysicsSystem.h>
#include <Physics/Ray.h>
#include <Vulkan/Common/MeshObject.h>
#include <Vulkan/Systems/GraphicsPipeline.h>
#include <Vulkan/Systems/GraphicsPipelineFactory.h>

#include "spdlog/spdlog.h"


static ImGuizmo::MODE currentGizmoMode(ImGuizmo::WORLD);
static ImGuizmo::OPERATION currentGizmoOperation(ImGuizmo::TRANSLATE);

Scene::~Scene()
{
	// do nothing
}

Scene::Scene(IDebugRegistry* inDebugRegistry)
{
	debugRegistry = inDebugRegistry;
}

void Scene::PreConstruct(const char* aSceneName)
{
	m_sceneName = aSceneName;

	physicsSystem = eastl::make_unique<PhysicsSystem>();
	sceneInteractionPhysicsSystem = eastl::make_unique<PhysicsSystem>();
	meshAllocator = eastl::make_unique<MeshAllocator>();

	physicsSystem->Create();
	sceneInteractionPhysicsSystem->Create();
	gInputSystem->RegisterMouseInput([&](const SDL_MouseMotionEvent& motion) { MouseMovement(motion); },
	                                 "Scene Mouse Movement");
	gInputSystem->RegisterMouseInput([&](const SDL_MouseButtonEvent& input) { MouseInput(input); },
	                                 "Scene Mouse Press");

	gInputSystem->RegisterKeyCodeInput(SDLK_W,
	                                   [this](SDL_KeyboardEvent) {
		                                   if (!activeCamera->IsCameraConsumingInput())
		                                   {
			                                   ChangeImGuizmoOperation(ImGuizmo::TRANSLATE);
		                                   }
	                                   }, "Scene Gizmo Translate");

	gInputSystem->RegisterKeyCodeInput(SDLK_E,
	                                   [this](SDL_KeyboardEvent) {
		                                   if (!activeCamera->IsCameraConsumingInput())
			                                   ChangeImGuizmoOperation(ImGuizmo::ROTATE);
	                                   }, "Scene Gizmo Rotate");

	gInputSystem->RegisterKeyCodeInput(SDLK_R,
	                                   [this](SDL_KeyboardEvent) {
		                                   if (!activeCamera->IsCameraConsumingInput())
			                                   ChangeImGuizmoOperation(ImGuizmo::SCALE);
	                                   }, "Scene Gizmo Scale");
}


void Scene::MouseMovement(const SDL_MouseMotionEvent& aMouseMotion)
{
	mouseX = static_cast<int>(aMouseMotion.x);
	mouseY = static_cast<int>(aMouseMotion.y);
}

void Scene::MouseInput(const SDL_MouseButtonEvent& aMouseInput)
{
	if (activeCamera->IsCameraConsumingInput())
		return;

	if (aMouseInput.button == SDL_BUTTON_LEFT
	    && aMouseInput.type == SDL_EVENT_MOUSE_BUTTON_DOWN
	    && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
	    && !ImGuizmo::IsOver())
	{
		const auto pickedRigidBody = PickRigidBody(mouseX, mouseY);
		for (const auto& object : sceneObjects)
		{
			if (ColliderComponent comp; object->GetComponent<ColliderComponent>(comp)
			                            && pickedRigidBody == comp.GetRigidBody())
			{
				pickedEntity = object.get();
				break;
			}
		}
	}
}

void Scene::Construct()
{
	for (const auto& obj : sceneObjects)
	{
		obj->Construct();
	}
	assert(activeCamera != nullptr);
}

void Scene::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                   uint32_t aCurrentFrame)
{
	for (const auto obj : renderPipelines)
	{
		obj->graphicsPipeline->Draw(aCommandBuffer, *this);
	}
}

void Scene::TickPhysics(float deltaTime)
{
	physicsSystem->Tick(deltaTime);
}

void Scene::DrawObjectsRecursive(SceneObject& entityToDraw)
{
	ImGui::PushID(&entityToDraw);

	ImGuiTreeNodeFlags nodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (entityToDraw.transform.GetChildCount() == 0)
		nodeFlag = ImGuiTreeNodeFlags_Leaf;

	if (pickedEntity == &entityToDraw)
	{
		nodeFlag |= ImGuiTreeNodeFlags_Selected;
		ImGui::Begin("Picked Object");
		ImGui::Text(pickedEntity->name.data());
		ImGui::PushID(&entityToDraw);
		entityToDraw.OnDebugGui();
		ImGui::PopID();
		ImGui::End();
	}

	if (ImGui::TreeNodeEx(entityToDraw.name.data(), nodeFlag))
	{
		for (auto childTransform : entityToDraw.transform.GetChildren())
		{
			SceneObject& childEntity = *sceneTransformRelationships.at(childTransform);
			DrawObjectsRecursive(childEntity);
		}
		ImGui::TreePop();
	}

	if (ImGui::IsItemClicked())
	{
		pickedEntity = &entityToDraw;
	}
	ImGui::PopID();
}

bool Scene::IsParentOfPickedEntity(const SceneObject& obj)
{
	return false;
}

void Scene::ChangeImGuizmoOperation(const int aOperation)
{
	currentGizmoOperation = static_cast<ImGuizmo::OPERATION>(aOperation);
}

void Scene::OnDebugGui()
{
	ImGui::Begin(m_sceneName.data());

	const ImGuiIO& io = ImGui::GetIO();
	ImGui::SeparatorText("Controls");
	ImGui::BeginChild("Controls",
	                  ImVec2(0.0f, 0.0f),
	                  ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY); {
		if (ImGui::RadioButton("Translate", currentGizmoOperation == ImGuizmo::TRANSLATE))
			currentGizmoOperation = ImGuizmo::TRANSLATE;

		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", currentGizmoOperation == ImGuizmo::ROTATE))
			currentGizmoOperation = ImGuizmo::ROTATE;

		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", currentGizmoOperation == ImGuizmo::SCALE))
			currentGizmoOperation = ImGuizmo::SCALE;

		if (currentGizmoOperation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", currentGizmoMode == ImGuizmo::LOCAL))
				currentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", currentGizmoMode == ImGuizmo::WORLD))
				currentGizmoMode = ImGuizmo::WORLD;
		}
	}
	ImGui::EndChild();

	if (ImGui::CollapsingHeader("Scene Information"))
	{
		ImGui::Indent();
		if (ImGui::CollapsingHeader("Lighting"))
		{
			ImGui::ColorEdit3("Point Light Color", &sceneData.color[0]);
			ImGui::DragFloat("Point Light Intensity", &sceneData.lightIntensity, 0.0125f);
			ImGui::DragFloat("Ambient Lighting", &sceneData.ambientStrength, 0.1f);
		}
		if (ImGui::CollapsingHeader("Camera"))
		{
			activeCamera->OnDebugGui();
		}
		ImGui::Unindent();
	}

	if (pickedEntity != nullptr)
	{
		// Draw Gizmos
		glm::mat4 matrix = pickedEntity->transform.GetWorldMatrix();

		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::AllowAxisFlip(false);
		if (ImGuizmo::Manipulate(glm::value_ptr(activeCamera->GetViewMatrix()),
		                         glm::value_ptr(activeCamera->GetPerspectiveMatrix()),
		                         currentGizmoOperation, currentGizmoMode,
		                         glm::value_ptr(matrix), nullptr))
		{
			pickedEntity->transform.SetLocalMatrix(matrix);
		}
	}

	if (ImGui::CollapsingHeader("Objects"))
	{
		for (const auto& obj : sceneObjects)
		{
			ImGui::BeginGroup();
			ImGui::Indent(); {
				if (obj->transform.GetParent() == nullptr)
				{
					DrawObjectsRecursive(*obj);
				}
			}
			ImGui::Unindent();
			ImGui::EndGroup();
		}
	}


	ImGui::SeparatorText("Statistics");
	ImGui::BeginChild("Statistics",
	                  ImVec2(0.0f, 0.0f),
	                  ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY); {
		ImGui::Text("FPS: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(static_cast<int32_t>(gGraphics->GetFps())).c_str());
		ImGui::SameLine();


		// Plot line crashes in msvc with address san on
		//ImGui::PlotLines("##fpsHistory", &gGraphics->GetFpsHistory().buffer.front(),
		//                 gGraphics->GetFpsHistory().buffer.size());


		ImGui::Text("Delta Time: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(gGraphics->DeltaTimeUnscaled()).c_str());

		ImGui::Text("Objects: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(static_cast<int32_t>(sceneObjects.size())).c_str());
		ImGui::Text("Graphic Systems: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(static_cast<int32_t>(renderPipelines.size())).c_str());
		if (ImGui::CollapsingHeader("Graphic Systems Info"))
		{
			if (ImGui::Button("Rebuild All"))
			{
				gGraphics->vulkanRenderer->SubmitEndOfFrameTask([this] {
					for (const auto& renderSystem : renderPipelines)
					{
						vkDeviceWaitIdle(gGraphics->logicalDevice);
						renderSystem->graphicsPipeline->Destroy();
						renderSystem->Create(renderSystem->GetBoundDescriptors());
						renderSystem->graphicsPipeline->Create();
					}
				});
			}
			ImGui::Indent();
			for (const auto system : renderPipelines)
			{
				if (ImGui::CollapsingHeader(system->graphicsPipeline->pipelineName))
				{
					ImGui::Indent();
					ImGui::Text("Material Count: ");
					ImGui::SameLine();
					//ImGui::Text(eastl::to_string(system->graphicsPipeline->renderers.size()).c_str());
					for (const auto& renderer : system->graphicsPipeline->renderers)
					{
						ImGui::Text(renderer->GetMaterial(0)->materialName.data());
					}
					ImGui::Unindent();
				}
			}
			ImGui::Unindent();
		}
	}
	ImGui::EndChild();

	ImGui::End();
}

void Scene::Tick(const float deltaTime)
{
	sceneInteractionPhysicsSystem->Tick(deltaTime);
	for (const auto& obj : sceneObjects)
	{
		obj->Tick(deltaTime);
	}
}

void Scene::Destroy()
{
	SPDLOG_INFO("Cleaning up scene {}", m_sceneName);
	for (const auto& obj : sceneObjects)
	{
		obj->Destroy();
	}
	activeCamera.reset();

	for (const auto& system : renderPipelines)
	{
		system->graphicsPipeline->Destroy();
	}
	physicsSystem->Destroy();
	physicsSystem.reset();

	sceneInteractionPhysicsSystem->Destroy();
	sceneInteractionPhysicsSystem.reset();

	meshAllocator->Destroy();
	meshAllocator.reset();
}

void Scene::AddRenderPipeline(GraphicsPipelineFactory* inPipelineFactory)
{
	renderPipelines.push_back(inPipelineFactory);
	// todo this is wack
	inPipelineFactory->graphicsPipeline->Create();
}

// TODO: Make pointers managed
MeshObject* Scene::CreateObject(const eastl::string_view& inName, const eastl::string_view& inMeshPath, Material& inMaterial,
                                GraphicsPipeline& inPipeline, const glm::vec3& inPos, const glm::vec3& inRot,
                                const glm::vec3& inScale)
{
	auto* object = MakeEntity<MeshObject>();
	object->CreateObject(inMaterial, inName);
	object->GetRenderer().BindRenderer(inPipeline);
	object->GetRenderer().LoadMesh(meshAllocator.get(), (FileManagement::GetWorkingDirectory() + inMeshPath.data()).c_str());

	object->transform.SetLocalPosition(inPos);
	object->transform.SetLocalRotation(inRot);
	object->transform.SetLocalScale(inScale);

	return object;
}

void Scene::AttachSphereCollider(SceneObject& aEntity, const float radius, const float mass,
                                 const float friction) const
{
	auto sphereCollider = eastl::make_unique<ColliderComponent>();
	ColliderCreateInfo sphereColliderInfo{};
	sphereColliderInfo.collisionShape = new btSphereShape(radius);
	sphereColliderInfo.mass = mass;
	sphereColliderInfo.friction = friction;
	sphereCollider->Create(physicsSystem.get(), sphereColliderInfo);
	aEntity.AddComponent(eastl::move(sphereCollider));
}

void Scene::AttachBoxCollider(SceneObject& inEntity, const glm::vec3 halfExtents, const float mass,
                              const float friction) const
{
	auto boxCollider = eastl::make_unique<ColliderComponent>();
	ColliderCreateInfo boxColliderInfo{};
	boxColliderInfo.collisionShape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
	boxColliderInfo.mass = mass;
	boxColliderInfo.friction = friction;
	boxCollider->Create(physicsSystem.get(), boxColliderInfo);
	inEntity.AddComponent(eastl::move(boxCollider));
}

const btRigidBody* Scene::PickRigidBody(const int x, const int y) const
{
	const Ray ray = activeCamera->GetRayTo(x, y);
	const btVector3 rayFrom(CollisionHelper::GlmToBullet(ray.origin));
	const btVector3 rayTo(CollisionHelper::GlmToBullet(ray.origin + ray.direction * activeCamera->zFar));

	btCollisionWorld::ClosestRayResultCallback RayCallback(rayFrom, rayTo);

	physicsSystem->GetDynamicsWorld()->rayTest(rayFrom, rayTo, RayCallback);
	if (RayCallback.hasHit())
	{
		if (const btRigidBody* pickedBody = btRigidBody::upcast(RayCallback.m_collisionObject))
		{
			return pickedBody;
		}
	}

	return nullptr;
}

Texture* Scene::CreateTexture(const eastl::string_view& aName, const eastl::vector<eastl::string>& aPathsSet)
{
	if (!sceneTextures.contains(aName))
	{
		sceneTextures.insert_or_assign(aName, eastl::make_unique<Texture>());

		auto* texture = sceneTextures.at(aName).get();
		texture->LoadImagesFromDisk(aPathsSet);
		texture->Create();

		return texture;
	}

	SPDLOG_ERROR("Texture already exists {}", aName);
	return nullptr;
}

#define DEBUG_RENDER 1
SceneObject& Scene::MakeEntity()
{
	SceneObject& newObject = *sceneObjects.emplace_back(eastl::make_unique<SceneObject>());
#if DEBUG_RENDER
	debugRegistry->Register(&newObject);
#endif
	return newObject;
}