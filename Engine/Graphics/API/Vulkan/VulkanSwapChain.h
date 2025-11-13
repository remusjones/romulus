//
// Created by Remus on 11/12/2022.
//
#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

#include "../Base/Common/Buffers/AllocatedImage.h"

class VulkanGraphicsImpl;

class VulkanSwapChain {
public:
    void Initialize(VkDevice &aLogicalDevice,
                    VkPhysicalDevice &aPhysicalDevice,
                    VkSurfaceKHR &aSurface,
                    VkRenderPass &aRenderPass,
                    VulkanGraphicsImpl *aWindow);

    void RecreateSwapChain();

    void CreateSwapChain();

    void CreateFrameBuffers();

    void CreateImageViews();
    void CreateDepthBufferView();
    void CreateRenderPass();

    void Cleanup();


    VkPhysicalDevice mPhysicalDevice;
    VulkanGraphicsImpl *mApplication{};
    VkSwapchainKHR mSwapChain{};
    VkFormat mSwapChainImageFormat;
    VkExtent2D swapChainExtents{};
    std::vector<VkImage> mSwapChainImages;
    std::vector<VkImageView> mSwapChainImageViews;
    VkDevice mLogicalDevice;
    VkSurfaceKHR mSurface;
    std::vector<VkFramebuffer> mSwapChainFrameBuffers;
    VkRenderPass renderPass;

    VkImageView mDepthImageView;
    AllocatedImage mAllocatedDepthImage;
    VkFormat _depthFormat;
};
