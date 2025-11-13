//
// Created by Remus on 5/02/2024.
//

#pragma once
#include <atomic>
#include <string>
#include <thread>
#include <unordered_map>
#include <bits/fs_fwd.h>

#include "Objects/ImGuiLayer.h"


struct FileInfo
{
	std::string fileName;
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
	void CreateDirectoryMonitor(const std::string& aPath);
	void UpdateDirectoryMonitor();
	const std::unordered_map<std::string, FileInfo>& GetDirectoryContent() const;

	std::string GetMonitoredDirectory();
	static void CreateDirectorySnapshot(const std::string& aPath,
	                                    std::unordered_map<std::string, FileInfo>& aDirectoryContent);

private:
	void ValidateDirectorySnapshot(const std::unordered_map<std::string, FileInfo>& aDirectoryContent);
	// Draws Directory Content
	void ImportFile(const std::string& aPath, FileStatus aStatus);

public:
	void OnImGuiRender() override;

private:
	std::string mMonitoredDirectory;
	// Create map of directory
	std::unordered_map<std::string, FileInfo> mDirectoryContent;

	std::atomic<bool> mIsFinishedImporting = false;
	std::thread mImportingThread;
	std::mutex mImportingThreadMtx;
};
