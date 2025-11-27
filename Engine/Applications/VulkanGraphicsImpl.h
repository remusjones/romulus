//
// Created by Remus on 4/11/2021.
// Generic VulkanGraphicsImpl process to run the "project"
//
#pragma once
#include <memory>
#include <vector>
#include <SDL3/SDL_video.h>

#include <Types/DequeBuffer.h>
#include "IApplication.h"
#include "InputSystem.h"
#include "EASTL/unique_ptr.h"
#include "EASTL/vector.h"
#include "Objects/Editor.h"
#include "Vulkan/RomulusVulkanRenderer.h"
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

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const eastl::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const eastl::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const;
	QueueFamilyIndices GetQueueFamilyIndices() const { return familyIndices; }

	void Run() override;
	float DeltaTimeUnscaled() const { return deltaTime; }
	float GetFps() const { return fps; }
	DequeBuffer<float>& GetFpsHistory() { return fpsCircularBuffer; }

private:
	void InitializeImgui();

	void ShutdownImgui() const;
	void ShutdownVulkan() const;
	void ShutdownWindow() const;

	void Update();
	void Destroy();

	void CreateInstance();
	void DestroyInstance();

	void CreateDebugMessenger();
	void DestroyDebugMessenger();

	void CreateLogicalDevice();
	void DestroyLogicalDevice() const;

	void CreateSurface();
	void DestroySurface() const;

	void CreateGraphicsPipeline();
	void DestroyGraphicsPipeline();

	void CreateScenes();
	void DestroyScenes();

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

	eastl::vector<const char*> GetRequiredExtensions() const;

private:
	void InitializeWindow() override;
	void InitializeRenderer() override;


public:
	// todo: evaluate these external usages, and make getters
	eastl::unique_ptr<VulkanSwapChain> swapChain;
	VkInstance vulcanInstance = VK_NULL_HANDLE;
	VkDevice logicalDevice{};
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkSurfaceKHR surface            = VK_NULL_HANDLE;
	VmaAllocator allocator          = VK_NULL_HANDLE;

	eastl::unique_ptr<Editor> romulusEditor;
	eastl::unique_ptr<Scene> activeScene;
	eastl::unique_ptr<DebugManager> debugManager;
	eastl::unique_ptr<RomulusVulkanRenderer> vulkanRenderer;


	SDL_Window* window = nullptr; // TODO: Move to interface
	InputSystem inputManager;

private:
	QueueFamilyIndices familyIndices;

	// TODO: Move these to IApplication
	int windowWidth;
	int windowHeight;
	const char* windowTitle;

	float deltaTime;
	float fps;

	VkDescriptorPool imguiDescriptionPool;
	DequeBuffer<float> fpsCircularBuffer;
	eastl::vector<VkExtensionProperties> Extensions;
	VkQueue graphicsQueue;
	VkQueue PresentQueue;
	VkRenderPass renderPass{};

	void InitializePhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice aPhysicalDevice) const;
	bool CheckDeviceExtensionSupport(VkPhysicalDevice aPhysicalDevice) const;

	VkPhysicalDeviceFeatures deviceFeatures{};

	const eastl::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	const eastl::vector<const char*> validationLayers =
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
