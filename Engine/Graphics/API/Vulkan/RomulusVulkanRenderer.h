//
// Created by Remus on 6/11/2021.
//
#pragma once

#include <functional>
#include <queue>
#include <vector>
#include "VulkanSwapChain.h"
#include "Base/Common/Data/Vertex.h"
#include "Base/Common/Data/FrameData.h"
#include "EASTL/bitset.h"
#include "EASTL/queue.h"
#include "EASTL/vector.h"
#include "tracy/TracyVulkan.hpp"

class Material;
class Scene;
class GraphicsPipeline;

struct UploadContext
{
	VkFence uploadContext;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
};

// todo: abstract this to use a renderer base clase
class RomulusVulkanRenderer
{
public:
	void Initialize(const VkDevice& inLogicalDevice,
	                VulkanSwapChain* inSwapChain,
	                const VkPhysicalDevice& inPhysicalDevice,
	                const VkQueue& inGraphicsQueue,
	                const VkQueue& inPresentQueue
	);

	const FrameData& GetCurrentFrame() { return frameData[currentFrame]; }
	const FrameData& GetFrame(int32_t index) { return frameData[index]; }

	void SubmitBufferCommand(std::function<void(VkCommandBuffer cmd)>&& function) const;

	void SubmitEndOfFrameTask(std::function<void()>&& task);

	void QueueFrameBufferRebuild() { flags[c_rebuildBufferFlag] = true; }

	void CreateUploadContext();
	void CreateCommandBuffers();
	void CreateCommandPool();
	void DestroyCommandPool() const;
	void CreateDescriptorPool();
	void DrawFrame(Scene& activeScene);

	void CreateSyncObjects();
	void CreateTracy();
	void Cleanup();

private:
	void CleanupOldSyncObjects();

public:
	eastl::vector<VkFence> imagesInFlight;
	VulkanSwapChain* swapChain;

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkDescriptorPool descriptorPool;
	UploadContext uploadContext;

	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

private:

	static constexpr uint8_t c_rebuildBufferFlag = 0;
	eastl::bitset<1> flags;

	// Cached Variables for layouts
	VkDevice logicalDevice                      = nullptr;
	VkPhysicalDevice physicalDevice             = nullptr;
	VkPhysicalDeviceProperties deviceProperties = {};

	std::vector<FrameData> frameData;
	tracy::VkCtx* tracyContext;

	eastl::vector<VkFence> inFlightFencesToDestroy;
	eastl::vector<VkSemaphore> imageAvailableSemaphoresToDestroy;
	eastl::vector<VkSemaphore> mRenderFinishedSemaphoresToDestroy;
	eastl::vector<VkSemaphore> renderFinishedSemaphores;

	eastl::queue<std::function<void()>> endOfFrameTasks;
	size_t currentFrame = 0;
};
