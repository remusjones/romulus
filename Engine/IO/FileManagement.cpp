//
// Created by Remus on 9/12/2022.
//

#include "FileManagement.h"
#include <cstring>
#include <fstream>

#include <io.h>
#include <libloaderapi.h>

#include "EASTL/string.h"
#include "EASTL/vector.h"


eastl::vector<char> FileManagement::GetShaderFileData(const eastl::string& filename)
{
	eastl::string finalDirectory = GetWorkingDirectory();
	finalDirectory.append(sFileInformationSettings.pShaderDirectory);
	finalDirectory.append(filename);

	std::ifstream file(finalDirectory.data(), std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file");
	}

	const size_t fileSize = (size_t)file.tellg();
	eastl::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

eastl::string FileManagement::GetWorkingDirectory()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(nullptr, buffer, MAX_PATH);
	const eastl::string::size_type pos = eastl::string(buffer).find_last_of("\\/");
	return eastl::string(buffer).substr(0, pos);
}

eastl::string FileManagement::MakeAssetPath(const eastl::string& relativePath)
{
	eastl::string basePath = GetAssetPath();
	basePath.append(relativePath);
	return basePath;
}

eastl::string FileManagement::GetAssetPath()
{
	eastl::string path = GetWorkingDirectory();
	path.append(sFileInformationSettings.pAssetPath);
	return path;
}

bool FileManagement::FileExists(const eastl::string& absolutePath)
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

eastl::vector<char> FileManagement::GetShaderFileDataPath(const char* directory)
{
	eastl::string finalDirectory = GetWorkingDirectory();
	finalDirectory.append(directory);

	std::ifstream file(finalDirectory.data(), std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file" + std::string(directory));
	}

	const size_t fileSize = file.tellg();
	eastl::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}
