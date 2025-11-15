//
// Created by Remus on 17/12/2023.
//

#include "MeshObject.h"
#include "Base/Common/Material.h"
#include "Scenes/Scene.h"
#include "spdlog/spdlog.h"

void MeshObject::Construct()
{
	Entity::Construct();
}

void MeshObject::Tick(float deltaTime)
{
	Entity::Tick(deltaTime);
}

void MeshObject::Cleanup()
{
	meshRenderer.DestroyRenderer();
	Entity::Cleanup();
}

void MeshObject::CreateObject(
	Material& aMaterial,
	const char* aName)
{
	mName = aName;
	SPDLOG_INFO("Creating Object {}", mName);

	meshRenderer.material   = &aMaterial;
	meshRenderer.mTransform = &transform;
}

void MeshObject::OnImGuiRender()
{
	Entity::OnImGuiRender();
	meshRenderer.material->OnImGuiRender();
}
