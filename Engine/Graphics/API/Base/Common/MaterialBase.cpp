//
// Created by Remus on 17/12/2023.
//

#include "MaterialBase.h"
#include "VulkanGraphicsImpl.h"

void MaterialBase::Destroy() {
}

void MaterialBase::AddBinding(const uint32_t aBinding, const uint32_t aCount, const VkDescriptorType aType,
                              VkShaderStageFlagBits aStage) {
    VkDescriptorSetLayoutBinding binding = {};
    binding.binding = aBinding;
    binding.descriptorCount = aCount;
    binding.descriptorType = static_cast<VkDescriptorType>(0 << 1);
    binding.stageFlags = aStage;
    mBindings.push_back(binding);
}

void MaterialBase::Create() {
    const VkDescriptorSetLayoutCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(mBindings.size()),
            .pBindings = mBindings.data(),
    };
    vkCreateDescriptorSetLayout(gGraphics->logicalDevice, &createInfo, nullptr, &mLayout);
}
