//
// Created by Remus on 20/12/2023.
//

#include "AllocatedBuffer.h"

#include <Logger.h>
#include "VulkanGraphicsImpl.h"

AllocatedBuffer::AllocatedBuffer(const void* inData, const VkDeviceSize inBufferSize,
                                 const VkBufferUsageFlags inUsageFlags)
{
	void* data;
	Create(inBufferSize, inUsageFlags);
	//copy vertex data
	vmaMapMemory(gGraphics->allocator, allocation, &data);
	memcpy(data, inData, inBufferSize);
	vmaUnmapMemory(gGraphics->allocator, allocation);
}

AllocatedBuffer::~AllocatedBuffer() = default;

void AllocatedBuffer::AllocateBuffer(const void* inData, const VkDeviceSize inBufferSize,
                                     const VkBufferUsageFlags inUsageFlags)
{
	void* data;
	Create(inBufferSize, inUsageFlags);
	vmaMapMemory(gGraphics->allocator, allocation, &data);
	memcpy(data, inData, inBufferSize);
	vmaUnmapMemory(gGraphics->allocator, allocation);
}

void AllocatedBuffer::MapMemory(const VmaAllocator aVmaAllocator, const void* aData, VmaAllocation aAllocation,
                                VkDeviceSize aSize)
{
	void* data;
	vmaMapMemory(aVmaAllocator, aAllocation, &data);
	memcpy(data, aData, aSize);
	vmaUnmapMemory(aVmaAllocator, aAllocation);
}

VkBuffer AllocatedBuffer::GetBuffer() const { return mBuffer; }

VmaAllocation AllocatedBuffer::GetAllocation() const { return allocation; }

bool AllocatedBuffer::IsAllocated() const
{
	return mBuffer != nullptr || allocation != nullptr;
}

void AllocatedBuffer::Create(const VkDeviceSize aSize, const VkBufferUsageFlags aUsage)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size               = aSize;
	bufferInfo.usage              = aUsage;

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage                   = VMA_MEMORY_USAGE_AUTO;
	vmaallocInfo.flags                   = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
		VMA_ALLOCATION_CREATE_MAPPED_BIT;
	if (const VkResult result = vmaCreateBuffer(gGraphics->allocator, &bufferInfo, &vmaallocInfo,
	                                            &mBuffer,
	                                            &allocation, nullptr); result != VK_SUCCESS)
	{
		Logger::Log(spdlog::level::critical, "Failed to create AllocatedBuffer");
	}
}


void AllocatedBuffer::Destroy()
{
	vmaDestroyBuffer(gGraphics->allocator, mBuffer, allocation);
	mBuffer     = nullptr;
	allocation = nullptr;
}
