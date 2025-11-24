//
// Created by remus on 24/11/2025.
//


#pragma once
#include "Data/Mesh.h"
#include "EASTL/hash_map.h"
#include "EASTL/string_view.h"
#include "EASTL/unique_ptr.h"


class MeshAllocator
{
public:
    Mesh* LoadMesh(const eastl::string_view& inPath);

    void ReleaseMesh(const eastl::string_view& inPath);
    void Cleanup();

private:
    eastl::hash_map<eastl::string_view, eastl::unique_ptr<Mesh>> allocatedMeshMap;
};
