//
// Created by Remus on 3/02/2024.
//

#include "Editor.h"
#include "ProjectExplorer/DirectoryMonitor.h"
#include "FileManagement.h"
#include "imgui.h"
#include "Profiler.h"
#include "VulkanGraphicsImpl.h"
#include "Scenes/Scene.h"

void Editor::Create()
{
	directoryMonitor = std::make_unique<DirectoryMonitor>();
	directoryMonitor->CreateDirectoryMonitor(FileManagement::GetAssetPath());

	// Works around the path having a '/' at the end TODO: fix this
	directoryPath   = std::filesystem::path(FileManagement::GetAssetPath()).parent_path();
	contextBounds = std::filesystem::path(FileManagement::GetAssetPath()).parent_path();
	GetFilesInDirectory(directoryPath, filesInDirectory, nestedDirectories);
}


void Editor::OnImGuiRender()
{
	gGraphics->activeScene->OnImGuiRender();
	Profiler::GetInstance().OnImGuiRender();
	DrawContent();
}

void Editor::FileBrowser(const std::filesystem::path& path)
{
	ImGui::Text("Current path: %s", path.string().c_str());
	if (path.has_parent_path() && path != contextBounds)
	{
		if (ImGui::Button("Up"))
		{
			directoryPath = path.parent_path();
			GetFilesInDirectory(directoryPath, filesInDirectory, nestedDirectories);
		}
	}

	for (auto& entry : nestedDirectories)
	{
		ImGui::PushID(entry.c_str());
		if (ImGui::Button(entry.c_str()))
		{
			directoryPath = std::filesystem::path(path) / entry;
			GetFilesInDirectory(directoryPath, filesInDirectory, nestedDirectories);
		}
		ImGui::PopID();
	}
}

void Editor::DrawContent()
{
	ImGui::Begin("DirectoryBrowser");
	FileBrowser(directoryPath);
	ImGui::End();

	ImGui::Begin("File Display");

	if (ImGui::Button(GetUniqueLabel("Refresh Files")))
	{
		directoryMonitor->UpdateDirectoryMonitor();
	}

	ImGui::BeginTable("files", 2, ImGuiTableFlags_BordersOuterH |
	                  ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_BordersInnerV);

	for (const auto& entry : filesInDirectory)
	{
		// TODO: Extract directly from importer files instead of raw paths

		ImGui::Button((std::string("##") + entry.path().filename().string()).c_str(),
		              ImVec2(100, 100));
		ImGui::Text("%s", entry.path().filename().string().c_str());
		ImGui::TableNextColumn();
	}
	ImGui::EndTable();
	directoryMonitor->OnImGuiRender();
	ImGui::End();
}

void Editor::GetFilesInDirectory(const std::filesystem::path& path,
                                 std::vector<std::filesystem::directory_entry>& resultFilesInDirectory)
{
	resultFilesInDirectory.clear();
	for (auto& entry : std::filesystem::directory_iterator(path))
	{
		if (!is_directory(entry.status()))
		{
			resultFilesInDirectory.push_back(entry);
		}
	}
}

void Editor::GetFilesInDirectory(const std::filesystem::path& path,
                                 std::vector<std::filesystem::directory_entry>& resultFiles,
                                 std::vector<std::string>& resultDirectories)
{
	resultFiles.clear();
	resultDirectories.clear();

	for (auto& entry : std::filesystem::directory_iterator(path))
	{
		if (!is_directory(entry.status()))
		{
			resultFiles.push_back(entry);
		}
		else
		{
			resultDirectories.push_back(entry.path().filename().string());
		}
	}
}
