//
// Created by remus on 24/11/2025.
//


#pragma once
#include "Data/Mesh.h"
#include "EASTL/hash_map.h"
#include "EASTL/string.h"
#include "EASTL/string_view.h"
#include "EASTL/unique_ptr.h"

class MeshAllocator
{
public:
    Mesh* LoadMesh(const eastl::string_view& inPath);

    void ReleaseMesh(const eastl::string_view& inPath);
    void Destroy();

private:
    // todo should probably ref count these to know when its safe to dealloc
    eastl::hash_map<size_t, eastl::unique_ptr<Mesh>> allocatedMeshMap;
};
