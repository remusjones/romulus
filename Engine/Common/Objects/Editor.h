//
// Created by Remus on 3/02/2024.
//

#pragma once
#include "ImGuiDebugLayer.h"
#include "EASTL/vector.h"

class Editor : public ImGuiDebugLayer
{
public:
	void OnImGuiRender() override;
};

class ImGuiDebugInstance
{
public:
	static ImGuiDebugInstance& Get();
	ImGuiDebugInstance(const ImGuiDebugInstance&) = delete;
	ImGuiDebugInstance& operator=(const ImGuiDebugInstance&) = delete;

	void RegisterDebugLayer(ImGuiDebugLayer* inImGuiLayer);
	void DrawImGui();

private:
	ImGuiDebugInstance() = default;
	eastl::vector<ImGuiDebugLayer*> debugLayersToDraw;
};
