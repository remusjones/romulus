#pragma once
#include <vulkan/vulkan_core.h>

#include <Base/Common/Material.h>
#include <Base/Common/Buffers/AllocatedImage.h>

class Cubemap final : public Material
{
public:
	Cubemap(const eastl::string_view& inMaterialName) : Material(inMaterialName) { }
	void Create(MaterialBase* inBaseMaterial) override;
	void Destroy() override;

	VkSampler sampler;
	VkImageView imageView;
	AllocatedImage allocatedImage;
	VkDescriptorImageInfo descriptorImageInfo;
	VkDescriptorSet descriptorSet;
};

