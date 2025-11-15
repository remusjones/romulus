//
// Created by Remus on 27/12/2023.
//

#pragma once
#include <string>
#include <vector>
#include "AllocatedImage.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"

struct Color_RGBA;

class Texture
{
public:
	virtual ~Texture() = default;
	Texture()          = default;

	void LoadImagesFromDisk(const eastl::vector<eastl::string>& imagePaths);

	virtual void Create(VkFilter sampleFilter                   = VK_FILTER_LINEAR,
	                    VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);


	void Destroy() const;

	AllocatedImage allocatedImage;
	uint32_t imageCount = 1;
	eastl::vector<VkImageView> imageViews;
	VkSampler sampler;
	eastl::vector<VkDescriptorImageInfo> imageBufferInfos;
};
