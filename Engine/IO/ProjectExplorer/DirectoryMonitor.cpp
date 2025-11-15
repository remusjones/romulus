#include "DirectoryMonitor.h"

#include <filesystem>
#include <FileManagement.h>

#include "EASTL/hash_map.h"

void DirectoryMonitor::CreateDirectoryMonitor(const eastl::string& aPath)
{
	monitoredDirectory = aPath;
	UpdateDirectoryMonitor();
	std::lock_guard lock(importingThreadMutex);
	if (importingThread.joinable())
	{
		importingThread.join();
	}
}

void DirectoryMonitor::UpdateDirectoryMonitor()
{
	importingThread = std::thread([this] {
		eastl::hash_map<eastl::string, FileInfo> newDirectoryContent;
		CreateDirectorySnapshot(monitoredDirectory, newDirectoryContent);
		ValidateDirectorySnapshot(newDirectoryContent);
	});
}

eastl::hash_map<eastl::string, FileInfo> DirectoryMonitor::GetDirectoryContent() const
{
	return directoryContent;
}

eastl::string& DirectoryMonitor::GetMonitoredDirectory()
{
	return monitoredDirectory;
}

void DirectoryMonitor::CreateDirectorySnapshot(const eastl::string& aPath,
                                               eastl::hash_map<eastl::string, FileInfo>& directoryContent)
{
	for (const std::filesystem::path path = aPath.c_str(); const auto& entry :
	     std::filesystem::recursive_directory_iterator(path))
	{
		auto entryPath = eastl::string((char*)entry.path().c_str());
		if (!directoryContent.contains(entryPath))
		{
			auto fileInfo = FileInfo{
				(char*)entry.path().filename().c_str(),
				last_write_time(entry)
			};
			directoryContent.insert_or_assign(entryPath, fileInfo);
		}
	}
}

void DirectoryMonitor::ValidateDirectorySnapshot(const eastl::hash_map<eastl::string, FileInfo>& aDirectoryContent)
{
	// Compare content with existing content, and update if necessary
	for (const auto& entry : aDirectoryContent)
	{
		if (directoryContent.contains(entry.first))
		{
			// File exists
			if (directoryContent.at(entry.first).lastWriteTime != entry.second.lastWriteTime)
			{
				ImportFile(entry.first, FileStatus::MODIFIED);
			}
		} else
		{
			// File is new
			ImportFile(entry.first, FileStatus::CREATED);
		}
	}
	for (const auto& entry : directoryContent)
	{
		if (!aDirectoryContent.contains(entry.first))
		{
			// File is erased
			ImportFile(entry.first, FileStatus::ERASED);
		}
	}
	directoryContent = aDirectoryContent;
}

void DirectoryMonitor::ImportFile(const eastl::string& path, const FileStatus& status)
{
	switch (status)
	{
		case FileStatus::CREATED:
			// Logger::Log("File CREATED: " + aPath);
			break;
		case FileStatus::MODIFIED:
			// Logger::Log("File MODIFIED: " + aPath);
			break;
		case FileStatus::ERASED:
			// Logger::Log("File ERASED: " + aPath);
			break;
		default: ;
	}
}

void DirectoryMonitor::OnImGuiRender()
{
	if (importingThread.joinable())
	{
		std::lock_guard lock(importingThreadMutex);
		importingThread.join();
	}
}
