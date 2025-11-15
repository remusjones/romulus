//
// Created by Remus on 5/01/2024.
//

#include "Cubemap.h"

#include "LoadUtilities.h"
#include <FileManagement.h>

#include "spdlog/spdlog.h"

void Cubemap::Create(MaterialBase* inBaseMaterial)
{
	// TODO: make these configurable
	constexpr int mipLevel    = 1;
	constexpr VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;

	std::vector<std::string> cubemapImagePaths;

	cubemapImagePaths.push_back(FileManagement::MakeAssetPath("Textures/Skybox/MegaSunRight.png"));
	cubemapImagePaths.push_back(FileManagement::MakeAssetPath("Textures/Skybox/MegaSunLeft.png"));
	cubemapImagePaths.push_back(FileManagement::MakeAssetPath("Textures/Skybox/MegaSunTop.png"));
	cubemapImagePaths.push_back(FileManagement::MakeAssetPath("Textures/Skybox/MegaSunBottom.png"));
	cubemapImagePaths.push_back(FileManagement::MakeAssetPath("Textures/Skybox/MegaSunFront.png"));
	cubemapImagePaths.push_back(FileManagement::MakeAssetPath("Textures/Skybox/MegaSunBack.png"));

	const bool loadSuccess = LoadUtilities::LoadImagesFromDisk(gGraphics, cubemapImagePaths, allocatedImage,
	                                                           VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
	if (!loadSuccess)
	{
		throw std::runtime_error("Failed to load cubemap image(s)");
	}

	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;

	samplerCreateInfo.addressModeU     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.compareOp        = VK_COMPARE_OP_NEVER;
	samplerCreateInfo.mipLodBias       = 0.0f;
	samplerCreateInfo.mipmapMode       = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.minLod           = 0.0f;
	samplerCreateInfo.maxLod           = static_cast<float>(mipLevel);
	samplerCreateInfo.maxAnisotropy    = 4.0;
	samplerCreateInfo.anisotropyEnable = VK_TRUE;
	samplerCreateInfo.borderColor      = VK_BORDER_COLOR_INT_OPAQUE_WHITE;

	const VkResult samplerCreateResult = vkCreateSampler(gGraphics->logicalDevice, &samplerCreateInfo, nullptr,
	                                                     &sampler);
	if (samplerCreateResult != VK_SUCCESS)
	{
		SPDLOG_ERROR("Failed to create Cubemap Sampler");
	}

	VkImageViewCreateInfo view{};
	view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	view.format = format;
	view.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
	view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	view.subresourceRange.baseMipLevel = 0;
	view.subresourceRange.baseArrayLayer = 0;
	view.subresourceRange.layerCount = 6;
	view.subresourceRange.levelCount = mipLevel;
	view.image = allocatedImage.mImage;

	const VkResult imageCreateResult = vkCreateImageView(gGraphics->logicalDevice, &view, nullptr, &imageView);
	if (imageCreateResult != VK_SUCCESS)
	{
		SPDLOG_ERROR("Failed to create image view");
	}


	descriptorImageInfo.sampler     = sampler;
	descriptorImageInfo.imageView   = imageView;
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	AddBinding(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);
	AddBinding(1, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

	Material::Create(this);

	for (int i = 0; i < RomulusVulkanRenderer::MAX_FRAMES_IN_FLIGHT; i++)
	{
		SetBuffers(gGraphics->vulkanRenderer->GetFrame(i).sceneBuffer, 0, 0);
	}

	std::vector<VkDescriptorImageInfo> descriptorInfos;
	descriptorInfos.push_back(descriptorImageInfo);

	BindTexture(descriptorInfos, 1);
}

void Cubemap::Destroy()
{
	vkDestroyImageView(gGraphics->logicalDevice, imageView, nullptr);
	vkDestroySampler(gGraphics->logicalDevice, sampler, nullptr);
	vmaDestroyImage(gGraphics->allocator, allocatedImage.mImage, allocatedImage.mAllocation);
	Material::Destroy();
}
