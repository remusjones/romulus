//
// Created by remus on 24/11/2025.
//

#include "MeshAllocator.h"
#include "spdlog/spdlog.h"

Mesh* MeshAllocator::LoadMesh(const eastl::string_view& inPath)
{
    // Lazy Init the mesh if it doesn't exist
    if (allocatedMeshMap.contains(inPath))
    {
        return allocatedMeshMap.at(inPath).get();
    }

    auto addedResult = allocatedMeshMap.emplace(inPath, eastl::make_unique<Mesh>());

    if (!addedResult.second)
    {
        throw std::runtime_error("failed to open file");
    }


    Mesh* newMesh = addedResult.first->second.get();
    newMesh->LoadFromObject(inPath.data());

    return newMesh;
}

void MeshAllocator::ReleaseMesh(const eastl::string_view& inPath)
{
    if (allocatedMeshMap.contains(inPath))
    {
        allocatedMeshMap.at(inPath)->Destroy();
        allocatedMeshMap.erase(inPath);
    }
}

void MeshAllocator::Destroy()
{
    for (auto& meshPair : allocatedMeshMap)
    {
        meshPair.second->Destroy();
    }
    allocatedMeshMap.clear();
}
