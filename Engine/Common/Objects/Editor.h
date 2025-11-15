//
// Created by Remus on 3/02/2024.
//

#pragma once
#include <filesystem>
#include <memory>
#include <vector>
#include "ImGuiLayer.h"
#include "ProjectExplorer/DirectoryMonitor.h"

// todo: this will likely be deleted

class Editor : ImGuiLayer
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
	                                std::vector<std::string>& resultDirectories);

	std::unique_ptr<DirectoryMonitor> directoryMonitor;
	std::vector<std::filesystem::directory_entry> filesInDirectory;
	std::vector<std::string> nestedDirectories;
	std::filesystem::path directoryPath;
	std::filesystem::path contextBounds;
};
