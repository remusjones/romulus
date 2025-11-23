//
// Created by Remus on 20/12/2023.
//

#pragma once
#include "vk_mem_alloc.h"

class AllocatedBuffer {
public:
    AllocatedBuffer() = default;
    AllocatedBuffer(const void *inData, VkDeviceSize inBufferSize,
    VkBufferUsageFlags inUsageFlags);
    ~AllocatedBuffer();

    void Create(VkDeviceSize aSize, VkBufferUsageFlags aUsage);

    void AllocateBuffer(const void* inData, VkDeviceSize inBufferSize, VkBufferUsageFlags inUsageFlags);

    // Copy void* data to buffer
    static void MapMemory(VmaAllocator aVmaAllocator, const void *aData, VmaAllocation aAllocation, VkDeviceSize aSize);

    [[nodiscard]] VkBuffer GetBuffer() const;
    [[nodiscard]] VmaAllocation GetAllocation() const;
    [[nodiscard]] bool IsAllocated() const;

    void Destroy();

private:
    VkBuffer mBuffer = nullptr;
    VmaAllocation allocation = nullptr;
};
