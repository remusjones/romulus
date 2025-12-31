//
// Created by Remus on 7/11/2021.
//
#pragma once
#include <optional>
#include <cstdint>
#include <vulkan/vulkan_core.h>
#include <EASTL/vector.h>

#include "EASTL/optional.h"

struct QueueFamilyIndices
{
    eastl::optional<uint32_t> mGraphicsFamily;
    eastl::optional<uint32_t> mPresentFamily;

    [[nodiscard]] bool IsComplete() const
    {
        return mGraphicsFamily.has_value() && mPresentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR mCapabilities;
    eastl::vector<VkSurfaceFormatKHR> mFormats;
    eastl::vector<VkPresentModeKHR> mPresentModes;
};