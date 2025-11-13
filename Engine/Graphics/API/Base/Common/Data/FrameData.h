//
// Created by Remus on 23/12/2023.
//

#pragma once
#include "Base/Common/Buffers/AllocatedBuffer.h"


struct FrameData {

    VkCommandBuffer mCommandBuffer;
    VkCommandPool mCommandPool;
    VkSemaphore mRenderSemaphore;
    VkSemaphore mPresentSemaphore;
    VkFence mRenderFence;



    //AllocatedBuffer mCameraBuffer;
    AllocatedBuffer sceneBuffer;
};
