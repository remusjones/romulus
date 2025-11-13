//
// Created by Remus on 27/12/2023.
//

#pragma once
#include <string>
#include <vector>
#include "AllocatedImage.h"

struct Color_RGBA;

class Texture {
public:
    Texture() = default;

    void LoadImagesFromDisk(const std::vector<std::string> &imagePaths);

    virtual void Create(VkFilter sampleFilter = VK_FILTER_LINEAR,
                        VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);


    void Destroy() const;

    AllocatedImage allocatedImage;
    uint32_t imageCount = 1;
    std::vector<VkImageView> imageViews;
    VkSampler sampler;
    std::vector<VkDescriptorImageInfo> imageBufferInfos;
};
