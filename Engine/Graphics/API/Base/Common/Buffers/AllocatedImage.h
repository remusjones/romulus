//
// Created by Remus on 20/12/2023.
//

#pragma once
#include <vulkan/vulkan_core.h>
#include "AllocatedBuffer.h"

class AllocatedImage
{
public:
	VkImage mImage;
	VmaAllocation mAllocation;
};
