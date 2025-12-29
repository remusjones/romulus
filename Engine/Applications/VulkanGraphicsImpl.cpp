//
// Created by Remus on 4/11/2021.
//
#define VMA_IMPLEMENTATION
#define VK_USE_PLATFORM_WIN32_KHR
#define DEBUG_RENDER 1

#include <chrono>
#include <cstring>
#include <set>
#include <stdexcept>
#include <VulkanGraphicsImpl.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <imgui.h>
#include <ImGuizmo.h>
#include <InputSystem.h>
#include <Objects/Editor.h>
#include <Scenes/SandboxScene.h>
#include <Vulkan/Common/MeshObject.h>

#include "EASTL/set.h"
#include "spdlog/spdlog.h"
#include "tracy/Tracy.hpp"
#include "tracy/TracyVulkan.hpp"

VulkanGraphics* gGraphics = nullptr;

void VulkanGraphicsImpl::Run()
{
	InitializeWindow();
	InitializeRenderer();
	Update();
	Destroy();
}

void VulkanGraphicsImpl::InitializeImgui()
{
	// the size of the pool is very oversize, but it's copied from imgui demo itself.
	const VkDescriptorPoolSize pool_sizes[] =
	{
		{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
		{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
		{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	vkCreateDescriptorPool(logicalDevice, &pool_info, nullptr, &imguiDescriptionPool);

	ImGui::CreateContext();
	ImGui_ImplSDL3_InitForVulkan(window);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = vulcanInstance;
	init_info.PhysicalDevice = physicalDevice;
	init_info.Device = logicalDevice;
	init_info.Queue = graphicsQueue;
	init_info.DescriptorPool = imguiDescriptionPool;
	init_info.MinImageCount = 3; // Use swap chain's min image count
	init_info.ImageCount = 3;    // Use swap chain's actual image count
	//init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT; // Only if you use MSAA
	init_info.PipelineInfoMain.RenderPass = swapChain->GetRenderPass();
	ImGui_ImplVulkan_Init(&init_info);
}

void VulkanGraphicsImpl::ShutdownImgui() const
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	vkDestroyDescriptorPool(logicalDevice, imguiDescriptionPool, nullptr);
	ImGui::DestroyContext();
}

void VulkanGraphicsImpl::ShutdownVulkan() const
{
	vmaDestroyAllocator(allocator);
}

void VulkanGraphicsImpl::Update()
{
	// Start Clock for FPS Monitoring
	auto startTime = std::chrono::high_resolution_clock::now();
	auto fpsStartTime = std::chrono::high_resolution_clock::now();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// Create FPS Window Header

	const eastl::string fpsHeader = windowTitle + eastl::string(" | FPS: ");

	int frameCount = 0;
	SDL_Event event;
	bool bQuitting = false;

	// main loop
	while (!bQuitting)
	{
		ZoneScopedN("MainApplicationLoop");
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL3_ProcessEvent(&event);
			gInputSystem->ConsumeInput(&event);

			if (event.type == SDL_EVENT_QUIT)
			{
				bQuitting = true;
			}

			if (event.type == SDL_EVENT_WINDOW_RESIZED)
			{
				gGraphics->vulkanRenderer->QueueFrameBufferRebuild();
			}
		}

		if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED))
		{
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			ImGui::NewFrame();
			ImGuizmo::BeginFrame();
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(),
			                             ImGuiDockNodeFlags_PassthruCentralNode);

			{
				ZoneScopedN("InputSystem::Update");
				gInputSystem->Update();
			}


			{
				ZoneScopedN("Scene::TickPhysics");
				activeScene->TickPhysics(deltaTime);
			}

			{
				ZoneScopedN("Scene::Tick()");
				activeScene->Tick(deltaTime);
			}

#if DEBUG_RENDER
			{
				ZoneScopedN("DebugManager::DrawImGui");
				debugManager->DrawImGui();
			}
#endif

			{
				ZoneScopedN("ImGui::Render");
				ImGui::Render();
			}

			{
				ZoneScopedN("RomulasVulkanRenderer::DrawFrame");
				vulkanRenderer->DrawFrame(*activeScene);
			}

			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		// Calculate Frames-Per-Second
		frameCount++;
		auto currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>
				(currentTime - startTime).count();

		if (const auto elapsedTime = std::chrono::duration_cast<
				std::chrono::seconds>(currentTime - fpsStartTime).count();
			elapsedTime > 0)
		{
			fps = frameCount / elapsedTime;
			fpsCircularBuffer.AddElement(fps);
			frameCount = -1;
			fpsStartTime = currentTime;
		}
		startTime = currentTime;
		FrameMark;
	}
}

void VulkanGraphicsImpl::Destroy()
{
	SPDLOG_INFO("Destroying VulkanGraphicsImpl");

	if (logicalDevice != nullptr)
	{
		vkDeviceWaitIdle(logicalDevice);
	}

	DestroyScenes();
	ShutdownImgui();
	DestroyGraphicsPipeline();
	ShutdownVulkan();
	DestroyLogicalDevice();
	DestroyDebugMessenger();
	DestroySurface();
	DestroyInstance();

	DestroyWindow();
}

VulkanGraphicsImpl::VulkanGraphicsImpl(const char* inWindowTitle,
                                       const int inWindowWidth,
                                       const int inWindowHeight) : vulkanRenderer(nullptr), deltaTime(0), fps(0),
                                                                   imguiDescriptionPool(nullptr),
                                                                   fpsCircularBuffer(100), graphicsQueue(nullptr),
                                                                   PresentQueue(nullptr),
                                                                   debugMessenger(nullptr)
{
	windowTitle = inWindowTitle;
	windowWidth = inWindowWidth;
	windowHeight = inWindowHeight;
}

void VulkanGraphicsImpl::InitializeRenderer()
{
	gGraphics = this;

	CreateInstance();
	CreateDebugMessenger();
	CreateSurface();
	InitializePhysicalDevice();
	CreateLogicalDevice();

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = physicalDevice;
	allocatorInfo.device = logicalDevice;
	allocatorInfo.instance = vulcanInstance;
	vmaCreateAllocator(&allocatorInfo, &allocator);

	swapChain = eastl::make_unique<VulkanSwapChain>();
	swapChain->Initialize(logicalDevice,
						  physicalDevice,
						  surface,
						  renderPass,
						  *this);
	CreateGraphicsPipeline();
	InitializeImgui();

	CreateScenes();

	romulusEditor = eastl::make_unique<Editor>();
	debugManager = eastl::make_unique<DebugManager>();
	debugManager->Register(romulusEditor.get());

}

void VulkanGraphicsImpl::InitializeWindow()
{
	SPDLOG_INFO("Initializing Window");
	// We initialize SDL and create a window with it.
	SDL_Init(SDL_INIT_VIDEO);

	//create blank SDL window for our application
	window = SDL_CreateWindow(
		windowTitle,  //window title
		windowWidth,  //window width in pixels
		windowHeight, //window height in pixels
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
	);
}

void VulkanGraphicsImpl::DestroyWindow() const
{
	if (window != nullptr)
	{
		SDL_DestroyWindow(window);
	}
}

void VulkanGraphicsImpl::CreateInstance()
{
	vulkanRenderer = eastl::make_unique<RomulusVulkanRenderer>();
	SPDLOG_INFO("Creating Vulkan Instance");

	if (enableValidationLayers && !CheckValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = windowTitle;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.
			size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &
				debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	const auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	if (vkCreateInstance(&createInfo, nullptr, &vulcanInstance) !=
	    VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
}

void VulkanGraphicsImpl::DestroyInstance()
{
	vkDestroyInstance(vulcanInstance, nullptr);
}

void VulkanGraphicsImpl::CreateSurface()
{
	if (!SDL_Vulkan_CreateSurface(window, vulcanInstance, nullptr, &surface))
	{
		throw std::runtime_error("failed to create window surface");
	}
}

void VulkanGraphicsImpl::DestroySurface() const
{
	vkDestroySurfaceKHR(vulcanInstance, surface, nullptr);
}

void VulkanGraphicsImpl::CreateScenes()
{

	ZoneScopedN("VulkanGraphicsImpl::CreateScenes");

	activeScene = eastl::make_unique<SandboxScene>(debugManager.get());

	{
		ZoneScopedN("Scene::PreConstruct");
		activeScene->PreConstruct("Sandbox Scene");
	}
	{
		ZoneScopedN("Scene::Construct");
		activeScene->Construct();
	}
}

void VulkanGraphicsImpl::DestroyScenes()
{
	activeScene->Destroy();
	activeScene.reset();
}

void VulkanGraphicsImpl::CreateGraphicsPipeline()
{
	vulkanRenderer->Initialize(logicalDevice,
	                           swapChain.get(),
	                           physicalDevice,
	                           graphicsQueue,
	                           PresentQueue);

	vulkanRenderer->CreateSyncObjects();
	swapChain->CreateFrameBuffers();
}

void VulkanGraphicsImpl::DestroyGraphicsPipeline()
{
	vulkanRenderer->Destroy();
	swapChain->Destroy();

	swapChain.reset();
	vulkanRenderer.reset();
}

bool VulkanGraphicsImpl::CheckValidationLayerSupport() const
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

eastl::vector<const char*> VulkanGraphicsImpl::GetRequiredExtensions() const
{
	Uint32 count = 0;
	const auto windowExtensions = SDL_Vulkan_GetInstanceExtensions(&count);

	eastl::vector<const char*> extensions(windowExtensions, windowExtensions + count);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

void VulkanGraphicsImpl::PopulateDebugMessengerCreateInfo(
	VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
	                         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
	                         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

void VulkanGraphicsImpl::CreateDebugMessenger()
{
	if (!enableValidationLayers)
	{
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	if (auto result = CreateDebugUtilsMessengerEXT(vulcanInstance, &createInfo, nullptr,
	                                 &debugMessenger) != VK_SUCCESS)
	{
		SPDLOG_ERROR("Failed to setup Debug Messenger {}", result);
	}
}

void VulkanGraphicsImpl::DestroyDebugMessenger()
{
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(vulcanInstance, debugMessenger, nullptr);
	}
}

VkResult VulkanGraphicsImpl::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                          const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
                                                          const VkAllocationCallbacks* allocator,
                                                          VkDebugUtilsMessengerEXT* debugMessenger)
{
	PFN_vkCreateDebugUtilsMessengerEXT func =
			reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
				vkGetInstanceProcAddr(instance,
				                      "vkCreateDebugUtilsMessengerEXT")
			);

	if (func != nullptr)
	{
		return func(instance, createInfo, allocator, debugMessenger);
	}
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanGraphicsImpl::DestroyDebugUtilsMessengerEXT(
	VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* allocator)
{
	auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
		instance, "vkDestroyDebugUtilsMessengerEXT"));
	if (func != nullptr)
	{
		func(instance, debugMessenger, allocator);
	}
}

VkBool32 VulkanGraphicsImpl::DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	void* userData)
{
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		SPDLOG_ERROR("Validation Layer Error {}", callbackData->pMessage);
	}
	return VK_FALSE;
}

void VulkanGraphicsImpl::InitializePhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulcanInstance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support");
	}

	eastl::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(vulcanInstance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU");
	}
	else
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

		SPDLOG_DEBUG("Selecting Device {}", deviceProperties.deviceName);
	}
}

bool VulkanGraphicsImpl::IsDeviceSuitable(VkPhysicalDevice aPhysicalDevice) const
{
	const QueueFamilyIndices indices = FindQueueFamilies(aPhysicalDevice);
	const bool extensionsSupported = CheckDeviceExtensionSupport(aPhysicalDevice);
	bool swapChainAdequate = false;

	if (extensionsSupported)
	{
		const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(
			aPhysicalDevice);
		swapChainAdequate = !swapChainSupport.mFormats.empty() && !
		                    swapChainSupport.mPresentModes.empty();
	}

	return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}


bool VulkanGraphicsImpl::CheckDeviceExtensionSupport(
	VkPhysicalDevice aPhysicalDevice) const
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(aPhysicalDevice, nullptr,
	                                     &extensionCount, nullptr);

	eastl::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(aPhysicalDevice, nullptr,
	                                     &extensionCount,
	                                     availableExtensions.data());

	std::set<eastl::string> requiredExtensions(deviceExtensions.begin(),
	                                         deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

QueueFamilyIndices VulkanGraphicsImpl::FindQueueFamilies(
	VkPhysicalDevice device) const
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
	                                         nullptr);

	eastl::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
	                                         queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
		                                     &presentSupport);

		if (presentSupport)
		{
			indices.mPresentFamily = i;
		}

		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.mGraphicsFamily = i;
		}

		if (indices.IsComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

SwapChainSupportDetails VulkanGraphicsImpl::QuerySwapChainSupport(
	VkPhysicalDevice device) const
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
	                                          &details.mCapabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
	                                     nullptr);

	if (formatCount != 0)
	{
		details.mFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
		                                     details.mFormats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
	                                          &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.mPresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			device, surface, &presentModeCount, details.mPresentModes.data());
	}

	return details;
}

void VulkanGraphicsImpl::CreateLogicalDevice()
{
	SPDLOG_DEBUG("Creating Logical Device");
	familyIndices = FindQueueFamilies(physicalDevice);

	eastl::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	const eastl::set<uint32_t> uniqueQueueFamilies = {
		familyIndices.mGraphicsFamily.value(),
		familyIndices.mPresentFamily.value()
	};

	constexpr float queuePriority = 1.0f;
	for (const uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	const uint32_t graphicsFamilyIndex = familyIndices.mGraphicsFamily.value();

	queueCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;


	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.
		size());

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.
		size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice)
	    != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(logicalDevice, familyIndices.mPresentFamily.value(), 0,
	                 &PresentQueue);
	vkGetDeviceQueue(logicalDevice, graphicsFamilyIndex, 0, &graphicsQueue);
}

void VulkanGraphicsImpl::DestroyLogicalDevice() const
{
	vkDestroyDevice(logicalDevice, nullptr);
}
