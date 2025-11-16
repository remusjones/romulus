//
// Created by Remus on 3/02/2024.
//

#include "Editor.h"
#include "FileManagement.h"
#include "Profiler.h"
#include "VulkanGraphicsImpl.h"
#include "Scenes/Scene.h"

void Editor::OnImGuiRender()
{
	gGraphics->activeScene->OnImGuiRender();
	Profiler::GetInstance().OnImGuiRender();
}

ImGuiDebugInstance& ImGuiDebugInstance::Get()
{
	static ImGuiDebugInstance instance;
	return instance;
}

void ImGuiDebugInstance::RegisterDebugLayer(ImGuiDebugLayer* inImGuiLayer)
{
	debugLayersToDraw.push_back(inImGuiLayer);
}

void ImGuiDebugInstance::DrawImGui()
{
	for (auto* imGuiLayer : debugLayersToDraw)
	{
		imGuiLayer->OnImGuiRender();
	}
}