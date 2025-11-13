//
// Created by Remus on 3/02/2024.
//

#pragma once
#include <filesystem>
#include <memory>
#include <vector>
#include "ImGuiLayer.h"
#include "ProjectExplorer/DirectoryMonitor.h"


class Editor : ImGuiLayer
{
public:
	void Create();
	void OnImGuiRender() override;

	void FileBrowser(const std::filesystem::path& aPath);

private:
	// Draws Directory Content
	void DrawContent();
	void GetFilesInDirectory(const std::filesystem::path& aPath,
	                         std::vector<std::filesystem::directory_entry>& filesInDirectory);
	void GetFilesInDirectory(const std::filesystem::path& aPath,
	                         std::vector<std::filesystem::directory_entry>& filesInDirectory,
	                         std::vector<std::string>& aDirectoriesInDirectory);

	std::unique_ptr<DirectoryMonitor> mAssetDirectoryMonitor;
	std::vector<std::filesystem::directory_entry> mFilesInDirectory;
	std::vector<std::string> mDirectoriesInDirectory;
	std::filesystem::path mCurrentPath;
	std::filesystem::path mContextBounds;
};
