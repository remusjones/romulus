//
// Created by Remus on 11/12/2022.
//
#pragma once
#include <vector>
#include <Base/Common/Buffers/AllocatedImage.h>
#include <vulkan/vulkan_core.h>

#include "EASTL/vector.h"

class VulkanGraphicsImpl;

class VulkanSwapChain {
public:
    void Initialize(const VkDevice &inLogicalDevice,
                    const VkPhysicalDevice &inPhysicalDevice,
                    const VkSurfaceKHR &inSurface,
                    const VkRenderPass &inRenderPass,
                    VulkanGraphicsImpl& inWindow);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const eastl::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const eastl::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

    void RecreateSwapChain();

    void CreateSwapChain();
    void CreateFrameBuffers();
    void CreateImageViews();
    void CreateDepthBufferView();
    void CreateRenderPass();

    void Destroy();


    VkPhysicalDevice physicalDevice;
    VulkanGraphicsImpl *application{};
    VkSwapchainKHR mSwapChain{};
    VkFormat mSwapChainImageFormat;
    VkExtent2D swapChainExtents{};
    std::vector<VkImage> mSwapChainImages;
    std::vector<VkImageView> mSwapChainImageViews;
    VkDevice logicalDevice;
    VkSurfaceKHR surface;
    std::vector<VkFramebuffer> mSwapChainFrameBuffers;
    VkRenderPass renderPass;

    VkImageView mDepthImageView;
    AllocatedImage mAllocatedDepthImage;
    VkFormat _depthFormat;
};
