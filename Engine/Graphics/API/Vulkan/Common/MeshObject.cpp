//
// Created by Remus on 17/12/2023.
//

#include "MeshObject.h"
#include <Logger.h>
#include "Base/Common/Material.h"
#include "Scenes/Scene.h"

void MeshObject::Construct() {
    Entity::Construct();
}

void MeshObject::Tick(float aDeltaTime) {
    Entity::Tick(aDeltaTime);
}

void MeshObject::Cleanup() {
    meshRenderer.DestroyRenderer();
    Entity::Cleanup();
}

void MeshObject::CreateObject(
    Material& aMaterial,
    const char* aName) {
    mName = aName;
    Logger::Log(spdlog::level::info, (std::string("Creating object ") + mName).c_str());
    meshRenderer.material = &aMaterial;
    meshRenderer.mTransform = &transform;
}

void MeshObject::OnImGuiRender() {
    Entity::OnImGuiRender();
    meshRenderer.material->OnImGuiRender();
}