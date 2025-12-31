//
// Created by Remus on 5/02/2024.
//

#pragma once
#include "EASTL/string.h"

class AssetItem
{
public:
    virtual ~AssetItem() = default;

    virtual void Create(eastl::string aPath) = 0;

    virtual const eastl::string& Description() const { return mDescription; }
    virtual const eastl::string& GetID() const { return mID; }
    virtual const eastl::string& GetPath() const { return mPath; }

protected:
    virtual void CreateFromAssetFile(eastl::string aPath) = 0;
    virtual void CreateFromMetaFile(eastl::string aPath) = 0;

    eastl::string mID;
    eastl::string mPath;
    eastl::string mDescription;
};
