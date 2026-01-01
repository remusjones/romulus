//
// Created by Remus on 22/12/2023.
//

#include "TemplateScene.h"

#include "FileManagement.h"
#include "Base/Common/Color.h"
#include "Vulkan/Common/MeshObject.h"
#include "Vulkan/Common/Primative.h"
#include "Vulkan/Materials/Cubemap.h"
#include "Vulkan/Materials/DefaultMaterial.h"
#include "Vulkan/Renderers/SkyboxRenderer.h"

void TemplateScene::PreConstruct(const char* inSceneName)
{
	Scene::PreConstruct(inSceneName);
}

void TemplateScene::Construct()
{
	// Todo move rendering setup to a separate function

	// Create Render Pipelines
	GraphicsPipeline::DefaultPipelineConfigInfo(defaultPipelineConfig);

	// todo: do I need to actually allocate these
	pbrRenderPipeline = std::make_unique<PBRRenderSystem>();
	unlitRenderPipeline = std::make_unique<UnlitRenderSystem>();
	cubemapRenderPipeline = std::make_unique<SkyboxRenderSystem>();
	wireframeRenderPipeline = std::make_unique<WireframeRenderSystem>();
	lineRendererPipeline = std::make_unique<LineRenderSystem>();

	// Define Material Usages
	// todo: do I need to actually allocate these
	Material& monkeyTexturedMaterial = materialUnlitFactory.Create<DefaultMaterial>("Monkey Unlit");
	Material& unlitMaterial = materialUnlitFactory.Create<DefaultMaterial>("Unlit");
	Material& teapotMaterial = materialPBRFactory.Create<DefaultMaterial>("Teapot PBR");
	Material& lightMaterial = materialUnlitFactory.Create<DefaultMaterial>("Light Unlit");
	Material& sphereMaterial = materialPBRFactory.Create<DefaultMaterial>("Sphere PBR");
	Material& cubeMaterial = materialPBRFactory.Create<DefaultMaterial>("Cube PBR");
	cubemap = &genericMaterialFactory.Create<Cubemap>("Skybox Cubemap");

	// Make Materials
	materialUnlitFactory.Make();
	materialPBRFactory.Make();
	genericMaterialFactory.Make();

	//
	// Bind Materials
	//
	unlitRenderPipeline->Create(materialUnlitFactory.GetDescriptorLayouts());
	lineRendererPipeline->Create(materialUnlitFactory.GetDescriptorLayouts());
	pbrRenderPipeline->Create(materialPBRFactory.GetDescriptorLayouts());
	wireframeRenderPipeline->Create(materialUnlitFactory.GetDescriptorLayouts());

	std::vector<VkDescriptorSetLayout> mCubemapLayouts;
	mCubemapLayouts.push_back(cubemap->GetDescriptorLayout());
	cubemapRenderPipeline->Create(mCubemapLayouts);

	//
	// Create Objects
	// TODO: Create constructor helper to make this smaller?
	//
	monkey = CreateObject("Monkey",
	                      "/Assets/Models/monkey_smooth.obj", monkeyTexturedMaterial,
	                      *wireframeRenderPipeline->graphicsPipeline,
	                      glm::vec3(2, 0, -5),
	                      glm::vec3(0),
	                      glm::vec3(1.f));


	teapot = CreateObject("Teapot",
	                      "/Assets/Models/teapot.obj", teapotMaterial,
	                      *pbrRenderPipeline->graphicsPipeline,
	                      glm::vec3(2, 0, -20),
	                      glm::vec3(0),
	                      glm::vec3(0.1f));

	light = CreateObject("Light",
	                     "/Assets/Models/sphere.obj", lightMaterial,
	                     *unlitRenderPipeline->graphicsPipeline,
	                     glm::vec3(0, 0, 0),
	                     glm::vec3(0),
	                     glm::vec3(0.2));

	monkey->transform.SetParent(&light->transform);

	constexpr int uniformSphereCount = 3;
	for (int i = 0; i < uniformSphereCount; i++)
	{
		for (int j = 0; j < uniformSphereCount; j++)
		{
			for (int k = 0; k < uniformSphereCount; k++)
			{
				constexpr float sphereRadius = 0.5f;
				MeshObject* sphere = CreateObject("Physics Sphere",
				                                  "/Assets/Models/sphere.obj", sphereMaterial,
				                                  *pbrRenderPipeline->graphicsPipeline,
				                                  glm::vec3(i, j, k),
				                                  glm::vec3(0),
				                                  glm::vec3(sphereRadius));

				AttachSphereCollider(*sphere, sphereRadius, 0.25f, 0.5);
			}
		}
	}


	constexpr glm::vec3 floorScale(50, 0.5f, 50);
	floor = CreateObject("Floor", "/Assets/Models/cube.obj",
	                     cubeMaterial, *pbrRenderPipeline->graphicsPipeline,
	                     glm::vec3(0, -10, 0), glm::vec3(0), floorScale);

	AttachBoxCollider(*floor, floorScale, 0);

	const eastl::vector stoneSet{
		FileManagement::GetWorkingDirectory() + "/Assets/Textures/Stone/Stone Wall.png",
		FileManagement::GetWorkingDirectory() + "/Assets/Textures/Stone/Stone Wall_NRM.png"
	};
	const auto stoneTexture = CreateTexture("stoneTexture", stoneSet);
	teapotMaterial.BindTexture(stoneTexture->imageBufferInfos, DefaultMaterial::TEXTURE);

	//
	// Skybox TODO: Skybox Constructor Required
	//
	cubeMapMesh = MakeEntity<Primitive>("Skybox");
	auto* mSkyboxRenderer = new SkyboxRenderer();
	allocatedRenderers.push_back(mSkyboxRenderer);

	cubeMapMesh->renderer = mSkyboxRenderer; // todo inconsistent use of renderer allocations compared to meshrenderer here
	mSkyboxRenderer->SetMaterial(0, cubemap);
	mSkyboxRenderer->transform = &cubeMapMesh->transform;
	mSkyboxRenderer->BindRenderer(*cubemapRenderPipeline->graphicsPipeline);
	mSkyboxRenderer->LoadMesh(*meshAllocator, (FileManagement::GetWorkingDirectory() +
	                                 eastl::string("/Assets/Models/cube.obj")).c_str());

	//
	// Scene Camera
	//
	activeCamera = eastl::make_unique<FlyCamera>();
	activeCamera->Construct();
	activeCamera->transform.SetLocalPosition({0, 0, -5.0f});

	lineRendererEntity = MakeEntity<Primitive>("LineRenderer");
	lineRendererEntity->renderer = lineRenderer;

	lineRenderer = new LineRenderer();
	allocatedRenderers.push_back(lineRenderer);

	lineRenderer->mTransform = &lineRendererEntity->transform;
	lineRenderer->SetLinePositions(
		{
			glm::vec3(0, -10, 0),
			glm::vec3(0, 10, 0),
			glm::vec3(10, 10, 0),
			glm::vec3(10, -10, 0)
		});

	lineRenderer->SetMaterial(0, &unlitMaterial);
	lineRenderer->BindRenderer(*lineRendererPipeline->graphicsPipeline);

	//
	// Setup Draw Order
	//
	AddRenderPipeline(cubemapRenderPipeline.get());
	AddRenderPipeline(unlitRenderPipeline.get());
	AddRenderPipeline(pbrRenderPipeline.get());
	AddRenderPipeline(wireframeRenderPipeline.get());
	AddRenderPipeline(lineRendererPipeline.get());
	//
	// Construct Scene
	//
	Scene::Construct();
}


void TemplateScene::Tick(float deltaTime)
{

	deltaAccumulated += deltaTime / 5;
	light->GetRenderer().GetMaterial(0)->materialProperties.color =
			glm::vec4(sceneData.color.x, sceneData.color.y, sceneData.color.z, 1);

	sceneData.position = light->transform.GetLocalPosition();
	sceneData.viewMatrix = activeCamera->GetViewMatrix();
	sceneData.viewPos = glm::vec4(activeCamera->transform.GetLocalPosition(), 1.0f);
	sceneData.viewProjectionMatrix = activeCamera->GetPerspectiveMatrix();

	monkey->transform.RotateLocal(glm::vec3(0.0f, 1, 0), deltaTime / 5);


	lineRenderer->SetLinePositions({
		                               teapot->transform.GetWorldPosition(),
		                               monkey->transform.GetWorldPosition(),
		                               light->transform.GetWorldPosition(),
		                               floor->transform.GetWorldPosition()
	                               }, {
		                               Color::Red(),
		                               Color::Green(),
		                               Color::Blue(),
		                               Color::Yellow()
	                               });


	activeCamera->Tick(deltaTime);

	Scene::Tick(deltaTime);
}

void TemplateScene::Destroy()
{

	for (const auto& renderer : allocatedRenderers)
	{
		renderer->DestroyRenderer();
		delete renderer;
	}

	for (const auto& loadedTextures : sceneTextures)
	{
		loadedTextures.second->Destroy();
	}


	materialUnlitFactory.Destroy();
	materialPBRFactory.Destroy();
	genericMaterialFactory.Destroy();

	Scene::Destroy();
}

void TemplateScene::OnDebugGui()
{
	Scene::OnDebugGui();
}
