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
    VkExtent2D GetExtents() const { return swapChainExtents; }
    VkRenderPass GetRenderPass() const { return renderPass; }
    VkFramebuffer GetFrameBuffer(const size_t imageIndex) const { return mSwapChainFrameBuffers[imageIndex]; }

    VkSwapchainKHR GetSwapchain() const { return swapchains.front(); }
    eastl::vector<VkSwapchainKHR>& GetSwapchains() { return swapchains; }

    VkPresentInfoKHR GetPresentInfo(uint32_t& imageIndex, const eastl::vector<VkSemaphore>& semaphores);
    VkResult AcquireNextImage(VkSemaphore presentSemaphore, uint32_t& outImageIndex);

    size_t GetSwapchainImageCount() const { return swapChainImages.size(); }

    void Recreate();
    void CreateSwapChain();
    void CreateFrameBuffers();
    void CreateImageViews();
    void CreateDepthBufferView();
    void CreateRenderPass();
    void Destroy();

private:
    VkPhysicalDevice physicalDevice{};
    VulkanGraphicsImpl* application{};
    VkRenderPass renderPass{};
    VkFormat swapChainImageFormat{};
    VkExtent2D swapChainExtents{};
    VkDevice logicalDevice{};
    VkSurfaceKHR surface{};
    VkImageView mDepthImageView{};
    AllocatedImage mAllocatedDepthImage{};
    VkFormat depthFormat{};

private:

    eastl::vector<VkSwapchainKHR> swapchains;
    eastl::vector<VkImage> swapChainImages;
    eastl::vector<VkImageView> mSwapChainImageViews;
    eastl::vector<VkFramebuffer> mSwapChainFrameBuffers;

};
