#include "Texture.h"
#include <stdexcept>
#include "LoadUtilities.h"
#include <Logger.h>
#include "VulkanGraphicsImpl.h"
#include "Vulkan/Helpers/VulkanInitialization.h"


void Texture::LoadImagesFromDisk(const std::vector<std::string>& imagePaths)
{
	LoadUtilities::LoadImagesFromDisk(gGraphics, imagePaths, allocatedImage);
	imageCount = imagePaths.size();
}

void Texture::Create(VkFilter sampleFilter,
                     VkSamplerAddressMode samplerAddressMode)
{
	const VkSamplerCreateInfo samplerInfo =
		VulkanInitialization::SamplerCreateInfo(sampleFilter, samplerAddressMode);

	vkCreateSampler(gGraphics->logicalDevice, &samplerInfo, nullptr, &sampler);

	// Creating image views for each layer
	for (uint32_t i = 0; i < imageCount; ++i)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image                           = allocatedImage.mImage; // the VkImage object
		viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		viewInfo.format                          = VK_FORMAT_R8G8B8A8_SRGB;
		viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel   = 0;
		viewInfo.subresourceRange.levelCount     = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount     = imageCount;

		VkImageView imageView;
		if (vkCreateImageView(gGraphics->logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture image view!");
		}

		imageViews.push_back(imageView);

		VkDescriptorImageInfo imageBufferInfo;
		imageBufferInfo.sampler     = sampler;
		imageBufferInfo.imageView   = imageView;
		imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageBufferInfos.push_back(imageBufferInfo);
	}
}

void Texture::Destroy() const
{
	vmaDestroyImage(gGraphics->allocator, allocatedImage.mImage, allocatedImage.mAllocation);
	// Destroy each imageView
	for (const auto imageView : imageViews)
	{
		vkDestroyImageView(gGraphics->logicalDevice, imageView, nullptr);
	}
	vkDestroySampler(gGraphics->logicalDevice, sampler, nullptr);
}
