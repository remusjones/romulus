//
// Created by Remus on 9/12/2022.
//


#pragma once
#include <vector>
#include <string>
#include "FileManagementInfo.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"

class FileManagement
{
public:
	FileManagement() = delete;

	static eastl::vector<char> GetShaderFileData(const eastl::string& filename);
	static eastl::vector<char> GetShaderFileDataPath(const char* directory);
	static void SetFileInformationSettings(const FileManagementSettings& shaderInfo);
	static const FileManagementSettings& GetInfoSettings();
	static eastl::string GetWorkingDirectory();
	static eastl::string MakeAssetPath(const eastl::string& relativePath);
	static eastl::string GetAssetPath();
	static bool FileExists(const eastl::string& absolutePath);

private:
	static inline FileManagementSettings sFileInformationSettings;
};
