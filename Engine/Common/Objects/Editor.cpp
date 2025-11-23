//
// Created by Remus on 3/02/2024.
//

#include "Editor.h"
#include "FileManagement.h"
#include "Profiler.h"
#include "VulkanGraphicsImpl.h"
#include "Scenes/Scene.h"

void Editor::OnDebugGui()
{
	gGraphics->activeScene->OnDebugGui();
	Profiler::GetInstance().OnDebugGui();
}

void DebugManager::Register(IDebugabble* object)
{
	debugLayersToDraw.push_back(object);
}

void DebugManager::DrawImGui()
{

	// cleanup any invalid values before drawing
	eastl::erase(debugLayersToDraw, nullptr);

	for (const auto& debuggable : debugLayersToDraw)
	{
		debuggable->OnDebugGui();
	}
}