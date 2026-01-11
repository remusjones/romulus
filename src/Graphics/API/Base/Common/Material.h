//
// Created by Remus on 17/12/2023.
//

#pragma once

#include <unordered_map>
#include <glm/vec4.hpp>
#include <vulkan/vulkan_core.h>
#include <Base/Common/MaterialBase.h>
#include <Base/Common/Buffers/AllocatedBuffer.h>
#include <Base/Common/Buffers/Texture.h>

#include "EASTL/hash_map.h"
#include "EASTL/vector.h"

class Buffer;

struct MaterialProperties
{
	glm::vec4 color = glm::vec4(1, 1, 1, 1);
	// TODO: Input Images for these properties?
	float specularStrength = 0.4;
	float shininess = 1;
	float debugRenderState;
	float _pad;
};

class Material : public MaterialBase
{
public:
	Material(const eastl::string_view& materialName) :
		materialProperties(),
		materialName(materialName),
		descriptorSet(nullptr)
	{
	}

	virtual void Create();

	virtual void CreateProperties(uint32_t inBinding, const MaterialProperties& inMaterialProperties);

	virtual void BindTexture(const eastl::vector<VkDescriptorImageInfo>& textureInfo, uint8_t aBinding) const;

	void BindTexture(const VkDescriptorImageInfo& imageInfo, uint8_t inBinding) const;

	void AddBinding(uint32_t inBinding, uint32_t inCount,
	                VkDescriptorType inType, VkShaderStageFlagBits inShaderStage) override;

	virtual void SetBuffers(const AllocatedBuffer& inBuffer, uint8_t inBinding, uint8_t inIndex) const;

	VkDescriptorSet GetDescriptorSet() const { return descriptorSet; }
	VkDescriptorSetLayout GetDescriptorLayout() const { return layout; }

	void Destroy() override;

	MaterialProperties materialProperties;
	AllocatedBuffer propertiesBuffer;
	eastl::string_view materialName;

protected:
	VkDescriptorSetLayout layout = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSet;
	eastl::hash_map<uint8_t, VkWriteDescriptorSet> boundTextureMap;
};
