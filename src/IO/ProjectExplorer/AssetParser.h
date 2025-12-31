//
// Created by Remus on 7/02/2024.
//

#pragma once

#include "AssetItem.h"
#include "EASTL/hash_map.h"
#include "EASTL/string.h"


class AssetParser
{
public:
    void Parse(const eastl::string& aPath);
    void RegisterAssetType(const eastl::string& aExtension, const AssetItem& aAssetItem);

private:
    eastl::hash_map<eastl::string, AssetItem&> mAssociatedAssetTypes;
};
