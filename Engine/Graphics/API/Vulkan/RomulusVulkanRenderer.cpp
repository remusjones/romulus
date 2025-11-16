//
// Created by Remus on 6/11/2021.
//
#include <fstream>
#include "RomulusVulkanRenderer.h"

#include <imgui_impl_vulkan.h>

#include "VulkanGraphicsImpl.h"
#include <iostream>

#include "imgui.h"
#include "Base/Common/Material.h"
#include "Base/Common/Data/GPUCameraData.h"
#include "Base/Common/Data/GPUSceneData.h"
#include "Helpers/VulkanInitialization.h"
#include "Scenes/Scene.h"
#include "spdlog/spdlog.h"

void RomulusVulkanRenderer::Initialize(const VkDevice& inLogicalDevice,
                                       VulkanSwapChain* inSwapChain,
                                       const VkPhysicalDevice& inPhysicalDevice,
                                       const VkQueue& inGraphicsQueue,
                                       const VkQueue& inPresentQueue
)
{
    // Register the shader module
    logicalDevice = inLogicalDevice;
    swapChain = inSwapChain;
    graphicsQueue = inGraphicsQueue;
    presentQueue = inPresentQueue;
    physicalDevice = inPhysicalDevice;

    // Configuration info population
    vkGetPhysicalDeviceProperties(inPhysicalDevice, &deviceProperties);
    frameData.resize(MAX_FRAMES_IN_FLIGHT);
    CreateDescriptorPool();
    CreateCommandPool();
}

void RomulusVulkanRenderer::Cleanup()
{
    CleanupOldSyncObjects();

    vkDestroyCommandPool(logicalDevice, uploadContext.commandPool, nullptr);

    // Destroy Frame data
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(logicalDevice, frameData[i].mPresentSemaphore,
                           nullptr);
        vkDestroySemaphore(logicalDevice, frameData[i].mRenderSemaphore,
                           nullptr);
        vkDestroyFence(logicalDevice, frameData[i].mRenderFence, nullptr);

        vkDestroyCommandPool(logicalDevice, frameData[i].mCommandPool, nullptr);

        //mFrameData[i].mCameraBuffer.Destroy();
        frameData[i].sceneBuffer.Destroy();
    }
    for (auto semaphore : renderFinishedSemaphores) {
        vkDestroySemaphore(logicalDevice, semaphore, nullptr);
    }
    vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
    vkDestroyRenderPass(logicalDevice, swapChain->renderPass, nullptr);
}

void RomulusVulkanRenderer::SubmitBufferCommand(std::function<void(VkCommandBuffer cmd)>&& function) const
{
    VkCommandBuffer cmd = uploadContext.commandBuffer;
    const VkCommandBufferBeginInfo cmdBeginInfo = VulkanInitialization::CommandBufferBeginInfo(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    vkBeginCommandBuffer(cmd, &cmdBeginInfo);
    function(cmd);
    vkResetFences(logicalDevice, 1, &uploadContext.uploadContext);
    vkEndCommandBuffer(cmd);

    const VkSubmitInfo submit = VulkanInitialization::SubmitInfo(&cmd);


    vkQueueSubmit(graphicsQueue, 1, &submit, uploadContext.uploadContext);
    vkWaitForFences(logicalDevice, 1, &uploadContext.uploadContext, true, 9999999999);
    vkResetFences(logicalDevice, 1, &uploadContext.uploadContext);
    vkResetCommandPool(logicalDevice, uploadContext.commandPool, 0);
}
void RomulusVulkanRenderer::SubmitEndOfFrameTask(std::function<void()> && task)
{
    endOfFrameTasks.emplace(std::move(task));
}

void RomulusVulkanRenderer::CreateUploadContext()
{
    QueueFamilyIndices queueFamilies = gGraphics->GetQueueFamilyIndices();
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilies.mGraphicsFamily.value();

    if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &uploadContext.commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool");
    }

    const VkCommandBufferAllocateInfo cmdAllocInfo = VulkanInitialization::CommandBufferAllocateInfo(uploadContext
                                                                                                     .commandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    vkAllocateCommandBuffers(logicalDevice, &cmdAllocInfo, &uploadContext.commandBuffer);


    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkCreateFence(logicalDevice, &info, nullptr, &uploadContext.uploadContext);
    inFlightFencesToDestroy.push_back(uploadContext.uploadContext);
}

void RomulusVulkanRenderer::CreateCommandPool()
{
    SPDLOG_DEBUG("Creating Command Pool");

    for (int i = 0; i < frameData.size(); i++)
    {
        QueueFamilyIndices queueFamilies = gGraphics->GetQueueFamilyIndices();
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilies.mGraphicsFamily.value();
        if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &frameData[i].mCommandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool");
        }
    }

    CreateUploadContext();
    CreateCommandBuffers();
}

void RomulusVulkanRenderer::CreateCommandBuffers()
{
    for (int i = 0; i < frameData.size(); i++)
    {
        VkCommandBufferAllocateInfo allocInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = frameData[i].mCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, &frameData[i].mCommandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }
}

void RomulusVulkanRenderer::DestroyCommandPool() const {
    vkDestroyCommandPool(logicalDevice, uploadContext.commandPool, nullptr);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroyCommandPool(logicalDevice, frameData[i].mCommandPool, nullptr);
    }
}

void RomulusVulkanRenderer::CreateDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> sizes =
    {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100}
    };

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    createInfo.maxSets = 10;
    createInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
    createInfo.pPoolSizes = sizes.data();

    vkCreateDescriptorPool(logicalDevice, &createInfo, nullptr, &descriptorPool);

    for (auto& currentFrameData : frameData)
    {
        //mFrameData[i].mCameraBuffer = CreateBuffer(sizeof(GPUCameraData),
        //                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        //                                           VMA_MEMORY_USAGE_CPU_TO_GPU);

        currentFrameData.sceneBuffer = AllocatedBuffer();
        currentFrameData.sceneBuffer.Create(sizeof(GPUSceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }
}

bool semaphoresNeedToBeRecreated = false;

void RomulusVulkanRenderer::DrawFrame(Scene& activeScene)
{
    FrameData currentFrameData = frameData[currentFrame];
    vkWaitForFences(logicalDevice, 1, &currentFrameData.mRenderFence, VK_TRUE,
                    UINT64_MAX);

    uint32_t imageIndex;
    VkCommandBuffer currentCommandBuffer = currentFrameData.mCommandBuffer;
    VkResult result = vkAcquireNextImageKHR(logicalDevice,
                                            swapChain->mSwapChain,
                                            0,
                                            currentFrameData.mPresentSemaphore,
                                            VK_NULL_HANDLE,
                                            &imageIndex);


    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        swapChain->RecreateSwapChain();
        // Sync objects aren't atomic, so we have to regenerate them at the end of the current frame
        semaphoresNeedToBeRecreated = true;
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to acquire swap chain image");
    }

    // Only reset the fence if we are submitting work
    vkResetFences(logicalDevice, 1, &currentFrameData.mRenderFence);
    vkResetCommandBuffer(currentCommandBuffer, 0);


    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    VkClearValue depthClear;
    depthClear.depthStencil.depth = 1.f;
    VkClearValue clearValues[] = {clearColor, depthClear};

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = gGraphics->swapChain->renderPass;
    renderPassInfo.framebuffer = gGraphics->swapChain->mSwapChainFrameBuffers[
        imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = gGraphics->swapChain->swapChainExtents;
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = &clearValues[0];


    // ----BEGIN FRAME----
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(currentCommandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.width = static_cast<float>(gGraphics->swapChain->swapChainExtents.width);

    viewport.y = static_cast<float>(gGraphics->swapChain->swapChainExtents.height);
    viewport.height = -static_cast<float>(gGraphics->swapChain->swapChainExtents.height);

    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(currentCommandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = gGraphics->swapChain->swapChainExtents;
    vkCmdSetScissor(currentCommandBuffer, 0, 1, &scissor);


    vkCmdBeginRenderPass(currentCommandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
    activeScene.Render(currentCommandBuffer, imageIndex, currentFrame);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), currentCommandBuffer);
    vkCmdEndRenderPass(currentCommandBuffer);

    if (vkEndCommandBuffer(currentCommandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
    // ----END FRAME----

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
    {
        vkWaitForFences(logicalDevice, 1, &imagesInFlight[imageIndex],
                        VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    imagesInFlight[imageIndex] = currentFrameData.mRenderFence;

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {currentFrameData.mPresentSemaphore};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &currentFrameData.mCommandBuffer;

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[imageIndex]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo,
                      currentFrameData.mRenderFence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain->mSwapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (rebuildBuffer)
    {
        swapChain->RecreateSwapChain();

        DestroyCommandPool();
        CreateCommandPool();
        // Sync objects aren't atomic, so we have to regenerate them at the end of the current frame
        semaphoresNeedToBeRecreated = true;
        rebuildBuffer = false;
    }
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        swapChain->RecreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    if (semaphoresNeedToBeRecreated)
    {
        CreateSyncObjects();
        semaphoresNeedToBeRecreated = false;
    }
    while(!endOfFrameTasks.empty()) {
        auto task = std::move(endOfFrameTasks.front());
        task();
        endOfFrameTasks.pop();
    }
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void RomulusVulkanRenderer::CleanupOldSyncObjects()
{
    for (const auto& i : mRenderFinishedSemaphoresToDestroy)
    {
        vkDestroySemaphore(logicalDevice, i, nullptr);
    }
    mRenderFinishedSemaphoresToDestroy.clear();

    for (const auto& i : inFlightFencesToDestroy)
    {
        vkDestroyFence(logicalDevice, i, nullptr);
    }
    inFlightFencesToDestroy.clear();

    for (const auto& i : imageAvailableSemaphoresToDestroy)
    {
        vkDestroySemaphore(logicalDevice, i, nullptr);
    }
    imageAvailableSemaphoresToDestroy.clear();
}

void RomulusVulkanRenderer::CreateSyncObjects()
{
    for (auto& frameData : frameData)
    {
        inFlightFencesToDestroy.push_back(frameData.mRenderFence);
        imageAvailableSemaphoresToDestroy.push_back(frameData.mRenderSemaphore);
        mRenderFinishedSemaphoresToDestroy.push_back(frameData.mPresentSemaphore);
    }
    SPDLOG_DEBUG("Creating Semaphores and Fences");
    imagesInFlight.resize(swapChain->mSwapChainImages.size(), VK_NULL_HANDLE);
    renderFinishedSemaphores.resize(swapChain->mSwapChainImages.size());

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (size_t i = 0; i < swapChain->mSwapChainImages.size(); i++)
    {
        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr,
            &renderFinishedSemaphores[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swapchain semaphore!");
        }
    }

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr,
                              &frameData[i].mPresentSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr,
                              &frameData[i].mRenderSemaphore) != VK_SUCCESS ||
            vkCreateFence(logicalDevice, &fenceInfo, nullptr,
                          &frameData[i].mRenderFence) != VK_SUCCESS)
        {
            throw std::runtime_error(
                "failed to create synchronization objects for a frame!");
        }
    }
}
