//
// Created by remus on 24/11/2025.
//

#include "MeshAllocator.h"
#include "spdlog/spdlog.h"

Mesh* MeshAllocator::LoadMesh(const eastl::string_view& inPath)
{
    // Lazy Init the mesh if it doesn't exist
    eastl::hash<eastl::string_view> hasher = eastl::hash<eastl::string_view>();
    size_t hashedPath = hasher(inPath);

    if (allocatedMeshMap.contains(hashedPath))
    {
        return allocatedMeshMap.at(hashedPath).get();
    }

    auto addedResult = allocatedMeshMap.emplace(hashedPath, eastl::make_unique<Mesh>());
    SPDLOG_INFO("Allocating new mesh {}", inPath.data());

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
    eastl::hash<eastl::string_view> hasher = eastl::hash<eastl::string_view>();
    size_t hashedPath = hasher(inPath);

    if (allocatedMeshMap.contains(hashedPath))
    {
        allocatedMeshMap.at(hashedPath)->Destroy();
        allocatedMeshMap.erase(hashedPath);
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
