#include "DirectoryMonitor.h"

#include <filesystem>
#include <FileManagement.h>
#include <Logger.h>

void DirectoryMonitor::CreateDirectoryMonitor(const std::string& aPath)
{
	mMonitoredDirectory = aPath;
	UpdateDirectoryMonitor();
	std::lock_guard lock(mImportingThreadMtx);
	if (mImportingThread.joinable())
	{
		mImportingThread.join();
	}
}

void DirectoryMonitor::UpdateDirectoryMonitor()
{
	mImportingThread = std::thread([this]
	{
		mIsFinishedImporting = false;
		std::unordered_map<std::string, FileInfo> newDirectoryContent;
		CreateDirectorySnapshot(mMonitoredDirectory, newDirectoryContent);
		ValidateDirectorySnapshot(newDirectoryContent);
		mIsFinishedImporting = true;
	});
}

const std::unordered_map<std::string, FileInfo>& DirectoryMonitor::GetDirectoryContent() const
{
	return mDirectoryContent;
}

std::string DirectoryMonitor::GetMonitoredDirectory()
{
	return mMonitoredDirectory;
}

void DirectoryMonitor::CreateDirectorySnapshot(const std::string& aPath,
                                               std::unordered_map<std::string, FileInfo>& aDirectoryContent)
{
	for (const std::filesystem::path path = aPath; const auto& entry :
	     std::filesystem::recursive_directory_iterator(path))
	{
		if (!aDirectoryContent.contains(entry.path().string()))
		{
			aDirectoryContent[entry.path().string()] = FileInfo{
				entry.path().filename().string(),
				last_write_time(entry)
			};
		}
	}
}

void DirectoryMonitor::ValidateDirectorySnapshot(const std::unordered_map<std::string, FileInfo>& aDirectoryContent)
{
	// Compare content with existing content, and update if necessary
	for (const auto& entry : aDirectoryContent)
	{
		if (mDirectoryContent.contains(entry.first))
		{
			// File exists
			if (mDirectoryContent[entry.first].lastWriteTime != entry.second.lastWriteTime)
			{
				ImportFile(entry.first, FileStatus::MODIFIED);
			}
		}
		else
		{
			// File is new
			ImportFile(entry.first, FileStatus::CREATED);
		}
	}
	for (const auto& entry : mDirectoryContent)
	{
		if (!aDirectoryContent.contains(entry.first))
		{
			// File is erased
			ImportFile(entry.first, FileStatus::ERASED);
		}
	}
	mDirectoryContent = aDirectoryContent;
}

void DirectoryMonitor::ImportFile(const std::string& aPath, const FileStatus aStatus)
{
	switch (aStatus)
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
	if (mImportingThread.joinable())
	{
		std::lock_guard lock(mImportingThreadMtx);
		if (mIsFinishedImporting)
		{
			mImportingThread.join();
		}
	}
}
