//
// Created by Remus on 23/12/2023.
//

#pragma once
#include "Base/Common/Buffers/AllocatedBuffer.h"


struct FrameData
{
	VkCommandBuffer commandBuffer;
	VkCommandPool commandPool;
	VkSemaphore renderSemaphore;
	VkSemaphore presentSemaphore;
	VkFence renderFence;

	//AllocatedBuffer mCameraBuffer;
	AllocatedBuffer sceneBuffer;
};
