//
// Created by Remus on 17/12/2023.
//

#include "MeshObject.h"
#include "Base/Common/Material.h"
#include "Scenes/Scene.h"
#include "spdlog/spdlog.h"

void MeshObject::Construct()
{
	SceneObject::Construct();
}

void MeshObject::Tick(float deltaTime)
{
	SceneObject::Tick(deltaTime);
}

void MeshObject::Cleanup()
{
	meshRenderer.DestroyRenderer();
	SceneObject::Cleanup();
}

void MeshObject::CreateObject(
	Material& inMaterial,
	const eastl::string_view& inName)
{
	name = inName;
	SPDLOG_INFO("Creating Object {}", name);

	meshRenderer.SetMaterial(0, &inMaterial);
	meshRenderer.SetTransform(&transform);
}

void MeshObject::OnDebugGui()
{
	SceneObject::OnDebugGui();
	meshRenderer.GetMaterial(0)->OnDebugGui();
}
