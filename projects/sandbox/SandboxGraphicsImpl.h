//
// Created by remus on 31/12/2025.
//


#pragma once
#include "VulkanGraphicsImpl.h"


class SandboxGraphicsImpl final : public VulkanGraphicsImpl
{
public:
    SandboxGraphicsImpl(const char* inWindowTitle, int inWindowWidth, int inWindowHeight)
        : VulkanGraphicsImpl(inWindowTitle, inWindowWidth, inWindowHeight)
    {
    }

private:
    void CreateScenes() override;
};
