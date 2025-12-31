//
// Created by Remus on 3/02/2024.
//

#include "Editor.h"
#include "FileManagement.h"
#include "VulkanGraphicsImpl.h"
#include "Scenes/Scene.h"

void Editor::OnDebugGui()
{
	gGraphics->activeScene->OnDebugGui();
}

void DebugManager::Register(IDebuggable* object)
{
	debugLayersToDraw.insert(object);
}

void DebugManager::Unregister(IDebuggable* object)
{
	debugLayersToDraw.erase(object);
}

void DebugManager::DrawImGui()
{
	// cleanup any invalid values before drawing

	for (const auto& debuggable : debugLayersToDraw)
	{
		debuggable->OnDebugGui();
	}
}
