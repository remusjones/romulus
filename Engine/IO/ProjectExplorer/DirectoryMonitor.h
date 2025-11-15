//
// Created by Remus on 5/02/2024.
//

#pragma once
#include <thread>
#include <bits/fs_fwd.h>

#include "EASTL/hash_map.h"
#include "EASTL/string.h"
#include "Objects/ImGuiLayer.h"


struct FileInfo
{
	char* fileName;
	std::filesystem::file_time_type lastWriteTime;
};

enum class FileStatus
{
	CREATED,
	MODIFIED,
	ERASED
};

class DirectoryMonitor : ImGuiLayer
{
public:
	void CreateDirectoryMonitor(const eastl::string& aPath);
	void UpdateDirectoryMonitor();

	eastl::hash_map<eastl::string, FileInfo> GetDirectoryContent() const;

	eastl::string& GetMonitoredDirectory();
	static void CreateDirectorySnapshot(const eastl::string& aPath,
	                                    eastl::hash_map<eastl::string, FileInfo>& directoryContent);

private:
	void ValidateDirectorySnapshot(const eastl::hash_map<eastl::string, FileInfo>& aDirectoryContent);
	void ImportFile(const eastl::string& path, const FileStatus& status);

public:
	void OnImGuiRender() override;

private:
	eastl::string monitoredDirectory;
	eastl::hash_map<eastl::string, FileInfo> directoryContent;
	std::thread importingThread;
	std::mutex importingThreadMutex;
};
