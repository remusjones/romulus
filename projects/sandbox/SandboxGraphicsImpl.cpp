//
// Created by remus on 31/12/2025.
//

#include "SandboxGraphicsImpl.h"
#include "SandboxScene.h"

void SandboxGraphicsImpl::CreateScenes()
{

    ZoneScopedN("VulkanGraphicsImpl::CreateScenes");

    activeScene = eastl::make_unique<SandboxScene>(debugManager.get());

    {
        ZoneScopedN("Scene::PreConstruct");
        activeScene->PreConstruct("Sandbox Scene");
    }
    {
        ZoneScopedN("Scene::Construct");
        activeScene->Construct();
    }
}
