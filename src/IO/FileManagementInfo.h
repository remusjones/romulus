//
// Created by Remus on 15/12/2023.
//

#pragma once

typedef struct FileManagementSettingsInfo
{
    const char* pShaderVertexSuffix = "_v";
    const char* pShaderFragmentSuffix = "_f";
    const char* pShaderFileExtension= ".spv";
    const char* pShaderDirectory = "\\Shaders\\";
    const char* pAssetPath = "\\Assets\\";
} FileManagementSettings;

