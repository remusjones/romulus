//
// Created by Remus on 11/12/2022.
//

#include <stdexcept>
#include <iostream>
#include "VulkanSwapChain.h"

#include <SDL3/SDL_events.h>

#include <Logger.h>
#include "VulkanSystemStructs.h"
#include "VulkanGraphicsImpl.h"
#include "Helpers/VulkanInitialization.h"

void VulkanSwapChain::RecreateSwapChain()
{
	int width  = 0;
	int height = 0;
	SDL_GetWindowSize(mApplication->window, &width, &height);
	while (width == 0 || height == 0)
	{
		SDL_GetWindowSize(mApplication->window, &width, &height);
		//SDL_Event event;
		//SDL_WaitEvent(&event);
		Logger::Log(spdlog::level::info, "Window Minimized");
	}

	vkDeviceWaitIdle(mLogicalDevice);
	Cleanup();
	CreateSwapChain();
	CreateImageViews();
	CreateFrameBuffers();
}

void VulkanSwapChain::CreateSwapChain()
{
	const SwapChainSupportDetails swapChainSupport = mApplication->
		QuerySwapChainSupport(mPhysicalDevice);

	const VkSurfaceFormatKHR surfaceFormat = mApplication->ChooseSwapSurfaceFormat(
		swapChainSupport.mFormats);
	const VkPresentModeKHR presentMode = mApplication->ChooseSwapPresentMode(
		swapChainSupport.mPresentModes);
	const VkExtent2D extent = mApplication->ChooseSwapExtent(
		swapChainSupport.mCapabilities);

	uint32_t imageCount = swapChainSupport.mCapabilities.minImageCount + 1;

	if (swapChainSupport.mCapabilities.maxImageCount > 0 && imageCount >
		swapChainSupport.mCapabilities.maxImageCount)
	{
		imageCount = swapChainSupport.mCapabilities.maxImageCount;
	}
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = mSurface;

	createInfo.minImageCount    = imageCount;
	createInfo.imageFormat      = surfaceFormat.format;
	createInfo.imageColorSpace  = surfaceFormat.colorSpace;
	createInfo.imageExtent      = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices          = gGraphics->GetQueueFamilyIndices();
	const uint32_t queueFamilyIndices[] = {
		indices.mGraphicsFamily.value(), indices.mPresentFamily.value()
	};

	if (indices.mGraphicsFamily != indices.mPresentFamily)
	{
		createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices   = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices   = nullptr;
	}

	createInfo.preTransform   = swapChainSupport.mCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode    = presentMode;
	createInfo.clipped        = VK_TRUE;
	createInfo.oldSwapchain   = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(mLogicalDevice, &createInfo, nullptr, &mSwapChain)
		!= VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}
	mSwapChainImageFormat = surfaceFormat.format;
	swapChainExtents      = extent;
	mSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(mLogicalDevice, mSwapChain, &imageCount,
	                        mSwapChainImages.data());
}

void VulkanSwapChain::CreateFrameBuffers()
{
	Logger::Log(spdlog::level::info, "Creating Frame Buffers");
	mSwapChainFrameBuffers.resize(mSwapChainImageViews.size());
	for (size_t i = 0; i < mSwapChainImageViews.size(); i++)
	{
		VkImageView attachments[2];
		attachments[0] = mSwapChainImageViews[i];
		attachments[1] = mDepthImageView;


		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass      = renderPass;
		framebufferInfo.attachmentCount = 2;
		framebufferInfo.pAttachments    = attachments;
		framebufferInfo.width           = swapChainExtents.width;
		framebufferInfo.height          = swapChainExtents.height;
		framebufferInfo.layers          = 1;

		if (vkCreateFramebuffer(mLogicalDevice, &framebufferInfo, nullptr,
		                        &mSwapChainFrameBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void VulkanSwapChain::CreateImageViews()
{
	mSwapChainImageViews.resize(mSwapChainImages.size());
	for (size_t i = 0; i < mSwapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo           = {};
		createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image                           = mSwapChainImages[i];
		createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format                          = mSwapChainImageFormat;
		createInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel   = 0;
		createInfo.subresourceRange.levelCount     = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount     = 1;


		if (vkCreateImageView(mLogicalDevice, &createInfo, nullptr,
		                      &mSwapChainImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views");
		}
	}
	CreateDepthBufferView();
}

void VulkanSwapChain::CreateDepthBufferView()
{
	Logger::Log(spdlog::level::info, "Creating Depth Buffer View");
	const VkExtent3D depthImageExtent = {
		swapChainExtents.width,
		swapChainExtents.height,
		1
	};

	_depthFormat = VK_FORMAT_D32_SFLOAT;

	const VkImageCreateInfo imageInfo = VulkanInitialization::CreateImageInfo(
		_depthFormat,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;
	dimg_allocinfo.requiredFlags           = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	vmaCreateImage(gGraphics->allocator, &imageInfo, &dimg_allocinfo,
	               &mAllocatedDepthImage.mImage,
	               &mAllocatedDepthImage.mAllocation, nullptr);

	const VkImageViewCreateInfo createInfo =
		VulkanInitialization::ImageViewCreateInfo(_depthFormat,
		                                          mAllocatedDepthImage.mImage, VK_IMAGE_ASPECT_DEPTH_BIT);

	if (vkCreateImageView(mLogicalDevice, &createInfo, nullptr,
	                      &mDepthImageView) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create depth image view");
	}
}

void VulkanSwapChain::Cleanup()
{
	Logger::Log(spdlog::level::info, "Destroying Frame Buffer");
	for (const auto& swapChainFrameBuffer : mSwapChainFrameBuffers)
	{
		vkDestroyFramebuffer(mLogicalDevice, swapChainFrameBuffer, nullptr);
	}

	vmaDestroyImage(gGraphics->allocator, mAllocatedDepthImage.mImage,
	                mAllocatedDepthImage.mAllocation);
	vkDestroyImageView(mLogicalDevice, mDepthImageView, nullptr);

	for (const auto& swapChainImageView : mSwapChainImageViews)
	{
		vkDestroyImageView(mLogicalDevice, swapChainImageView, nullptr);
	}

	vkDestroySwapchainKHR(mLogicalDevice, mSwapChain, nullptr);
}


void VulkanSwapChain::CreateRenderPass()
{
	VkAttachmentDescription depthAttachment = {};

	// Depth attachment
	depthAttachment.flags          = 0;
	depthAttachment.format         = _depthFormat;
	depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout    =
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment            = 1;
	depthAttachmentRef.layout                =
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass          = 0;
	dependency.srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask       = 0;
	dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkSubpassDependency depthDependency = {};
	depthDependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
	depthDependency.dstSubpass          = 0;
	depthDependency.srcStageMask        =
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
		VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	depthDependency.srcAccessMask = 0;
	depthDependency.dstStageMask  =
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
		VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	depthDependency.dstAccessMask =
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


	VkAttachmentDescription colorAttachment{};
	colorAttachment.format         = mSwapChainImageFormat;
	colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass    = {};
	subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount    = 1;
	subpass.pColorAttachments       = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkAttachmentDescription attachments[2] = {colorAttachment, depthAttachment};
	VkSubpassDependency dependencies[2]    = {dependency, depthDependency};

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 2;
	renderPassInfo.dependencyCount = 2;
	renderPassInfo.pAttachments    = &attachments[0];
	renderPassInfo.pDependencies   = &dependencies[0];
	renderPassInfo.subpassCount    = 1;
	renderPassInfo.pSubpasses      = &subpass;


	if (vkCreateRenderPass(mLogicalDevice, &renderPassInfo, nullptr,
	                       &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

void VulkanSwapChain::Initialize(VkDevice& aLogicalDevice,
                                 VkPhysicalDevice& aPhysicalDevice,
                                 VkSurfaceKHR& aSurface,
                                 VkRenderPass& aRenderPass,
                                 VulkanGraphicsImpl* aWindow)
{
	mLogicalDevice  = aLogicalDevice;
	mPhysicalDevice = aPhysicalDevice;
	mSurface        = aSurface;
	renderPass      = aRenderPass;
	mApplication    = aWindow;

	Logger::Log(spdlog::level::debug, "Constructing Swap Chain");
	CreateSwapChain();
	Logger::Log(spdlog::level::debug, "Constructing Image Views");
	CreateImageViews();
	Logger::Log(spdlog::level::debug, "Creating Render Pass");
	CreateRenderPass();
}
