#pragma once
#include <vulkan/vulkan_core.h>

#include "Base/Common/Material.h"
#include "Base/Common/Buffers/AllocatedImage.h"

class Cubemap : public Material {
public:
    Cubemap(const char* aMaterialName) {materialName = aMaterialName;}
    void Create(MaterialBase *aBaseMaterial) override;
    void Destroy() override;

    VkSampler mSampler;
    VkDescriptorImageInfo mDescriptorImageInfo;
    VkImageView mImageView;
    AllocatedImage mAllocatedImage;

    VkDescriptorSet mDescriptorSet;

};

