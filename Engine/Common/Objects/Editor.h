//
// Created by Remus on 3/02/2024.
//

#pragma once
#include <filesystem>
#include <memory>
#include <vector>
#include "ImGuiDebugLayer.h"
#include "EASTL/vector.h"
#include "ProjectExplorer/DirectoryMonitor.h"

// todo: this will likely be deleted

class Editor : public ImGuiDebugLayer
{
public:
	void Create();
	void OnImGuiRender() override;
	void FileBrowser(const std::filesystem::path& path);

private:
	// Draws Directory Content
	void DrawContent();
	static void GetFilesInDirectory(const std::filesystem::path& path,
	                                std::vector<std::filesystem::directory_entry>& filesInDirectory);
	static void GetFilesInDirectory(const std::filesystem::path& path,
	                                std::vector<std::filesystem::directory_entry>& resultFiles,
	                                std::vector<eastl::string>& resultDirectories);

	std::unique_ptr<DirectoryMonitor> directoryMonitor;
	std::vector<std::filesystem::directory_entry> filesInDirectory;
	std::vector<eastl::string> nestedDirectories;
	std::filesystem::path directoryPath;
	std::filesystem::path contextBounds;
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
