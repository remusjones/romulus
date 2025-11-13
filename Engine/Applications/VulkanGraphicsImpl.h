//
// Created by Remus on 4/11/2021.
// Generic VulkanGraphicsImpl process to run the "project"
//
#pragma once
#include <memory>
#include <vector>
#include <SDL3/SDL_video.h>

#include "DequeBuffer.h"
#include "IApplication.h"
#include "InputSystem.h"
#include "Vulkan/VulkanEngine.h"
#include "Vulkan/VulkanSystemStructs.h"

class Editor;
class Scene;
class VulkanSwapChain;
class MeshObject;

// TODO: This should really only contain window information, and initial vulkan creation. Everything else
// TODO: should probably go within the Engine class
class VulkanGraphicsImpl final : public IApplication
{
public:
	VulkanGraphicsImpl(const char* inWindowTitle, int inWindowWidth, int inWindowHeight);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const;
	QueueFamilyIndices GetQueueFamilyIndices() const { return familyIndices; }

	void Run() override;
	float DeltaTimeUnscaled() const { return deltaTime; }
	float GetFps() const { return fps; }
	DequeBuffer& GetFpsHistory() { return fpsCircularBuffer; }

private:
	void InitializeVulkan();
	void InitializeImgui();

	void ShutdownImgui() const;
	void ShutdownVulkan() const;

	void InitializeWindow();
	void ShutdownWindow() const;
	void Update();
	void Cleanup();

	void CreateInstance();
	void DestroyInstance() const;

	void CreateDebugMessenger();
	void DestroyDebugMessenger();

	void CreateLogicalDevice();
	void DestroyLogicalDevice() const;

	void CreateSurface();
	void DestroySurface() const;

	void CreateGraphicsPipeline();
	void DestroyGraphicsPipeline();

	void CreateScenes();
	void DestroyScenes() const;

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData);
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
	                                      const VkAllocationCallbacks* allocator,
	                                      VkDebugUtilsMessengerEXT* debugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
	                                   const VkAllocationCallbacks* allocator);
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	bool CheckValidationLayerSupport() const;
	std::vector<const char*> GetRequiredExtensions() const;

public:
	// todo: evaluate these external usages, and make getters
	std::unique_ptr<VulkanSwapChain> swapChain;
	VkInstance vulcanInstance = VK_NULL_HANDLE;
	VkDevice logicalDevice{};
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VmaAllocator allocator = VK_NULL_HANDLE;

	std::unique_ptr<Scene> activeScene;
	SDL_Window* window = nullptr; // TODO: Move to interface
	VulkanEngine vulkanEngine;
	InputSystem inputManager;

private:
	std::unique_ptr<Editor> romulusEditor;
	QueueFamilyIndices familyIndices;

	// TODO: Move these to IApplication
	int windowWidth;
	int windowHeight;
	const char* windowTitle;


	float deltaTime;
	float fps;

	VkDescriptorPool imguiDescriptionPool;
	DequeBuffer fpsCircularBuffer;
	std::vector<VkExtensionProperties> Extensions;
	VkQueue graphicsQueue;
	VkQueue PresentQueue;
	VkRenderPass renderPass{};

	void InitializePhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice aPhysicalDevice) const;
	bool CheckDeviceExtensionSupport(VkPhysicalDevice aPhysicalDevice) const;

	VkPhysicalDeviceFeatures deviceFeatures{};

	const std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};

	VkDebugUtilsMessengerEXT debugMessenger;

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
};

typedef VulkanGraphicsImpl VulkanGraphics;
extern VulkanGraphics* gGraphics;
