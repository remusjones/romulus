//
// Created by Remus on 9/12/2022.
//

#include "FileManagement.h"
#include <cstring>
#include <fstream>

#include <io.h>
#include <libloaderapi.h>


std::vector<char> FileManagement::GetShaderFileData(const std::string& filename)
{
	std::string finalDirectory = GetWorkingDirectory();
	finalDirectory.append(sFileInformationSettings.pShaderDirectory);
	finalDirectory.append(filename);

	std::ifstream file(finalDirectory, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file " + filename);
	}

	const size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

std::string FileManagement::GetWorkingDirectory()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(nullptr, buffer, MAX_PATH);
	const std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}

std::string FileManagement::MakeAssetPath(const std::string& relativePath)
{
	std::string basePath = GetAssetPath();
	basePath.append(relativePath);
	return basePath;
}

std::string FileManagement::GetAssetPath()
{
	std::string path = GetWorkingDirectory();
	path.append(sFileInformationSettings.pAssetPath);
	return path;
}

bool FileManagement::FileExists(const std::string& absolutePath)
{
	return access(absolutePath.c_str(), 0) == 0;
}

void FileManagement::SetFileInformationSettings(
	const FileManagementSettings& shaderInfo)
{
	sFileInformationSettings = shaderInfo;
}

const FileManagementSettings& FileManagement::GetInfoSettings()
{
	return sFileInformationSettings;
}

std::vector<char> FileManagement::GetShaderFileDataPath(const char* directory)
{
	std::string finalDirectory = GetWorkingDirectory();
	finalDirectory.append(directory);

	std::ifstream file(finalDirectory, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file" + std::string(directory));
	}

	const size_t fileSize = file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}
