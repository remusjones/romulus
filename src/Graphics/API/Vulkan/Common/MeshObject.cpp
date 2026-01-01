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

void MeshObject::Destroy()
{
	meshRenderer.DestroyRenderer();
	SceneObject::Destroy();
}

void MeshObject::CreateObject(
	Material& inMaterial,
	const eastl::string_view& inName)
{
	name = inName;
	meshRenderer.SetMaterial(0, &inMaterial);
	meshRenderer.SetTransform(&transform);
}

void MeshObject::OnDebugGui()
{
	SceneObject::OnDebugGui();
	meshRenderer.GetMaterial(0)->OnDebugGui();
}
