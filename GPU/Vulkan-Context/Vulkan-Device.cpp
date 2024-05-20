#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"
#include <string.h>



static VkInstance g_instance = VK_NULL_HANDLE;

#if defined(RUN_DEBUG)
static VKAPI_ATTR VkBool32 VKAPI_CALL g_debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	printf("Validation Layer: %s\n", pCallbackData->pMessage);
	return VK_FALSE;
}
#endif



void GPUFixedContext::build_commandThread(uint32_t in_queueFamilyIndex, uint32_t in_queueIndex, GPUCommandQueue* in_queue, GPUCommandPool* in_pool, GPUCommandSet* in_set) {
	vkGetDeviceQueue(m_logical, in_queueFamilyIndex, in_queueIndex, in_queue);
	{
		const VkCommandPoolCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = in_queueFamilyIndex
		};
		CHECK(vkCreateCommandPool(m_logical, &CreateInfo, nullptr, in_pool))
	}
	const VkCommandBufferAllocateInfo AllocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = *in_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};
	CHECK(vkAllocateCommandBuffers(m_logical, &AllocInfo, in_set))
}

void GPUFixedContext::ruin_commandThread(GPUCommandPool* in_pool, GPUCommandSet* in_set) {
	vkFreeCommandBuffers(m_logical, *in_pool, 1, in_set);
	vkDestroyCommandPool(m_logical, *in_pool, nullptr);
}



void GPUFixedContext::build_device(GLFWwindow* in_window, GPUExtent3D in_extent) {
	uint32_t HelperCount = 0;
	
	volkInitialize();
	{
		const VkApplicationInfo AppInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "Temper",
			.applicationVersion = VK_VERSION_1_0,
			.pEngineName = "TemperLogic",
			.engineVersion = VK_VERSION_1_0,
			.apiVersion = VK_API_VERSION_1_3
		};

#if defined(RUN_DEBUG)
		const char* Validation = "VK_LAYER_KHRONOS_validation";
		const VkDebugUtilsMessengerCreateInfoEXT DebugInfo = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.pNext = nullptr,
			.flags = 0,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
			.pfnUserCallback = &g_debugCallback
		};
#endif

		uint32_t SwapchainColourSpaceExtension = 0;
		{
			CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &HelperCount, nullptr))
			auto ExtensionProperties = new VkExtensionProperties[HelperCount];
			CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &HelperCount, ExtensionProperties))
			for(uint32_t i = 0; i < HelperCount; i++) if(!strcmp(ExtensionProperties[i].extensionName, VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME)) SwapchainColourSpaceExtension = 1;
			delete[] ExtensionProperties;
		}

		const char** RequiredExtensions = glfwGetRequiredInstanceExtensions(&HelperCount);
		HelperCount += SwapchainColourSpaceExtension;
#if defined(RUN_DEBUG)
		HelperCount++;
#endif
		auto Extensions = new const char*[HelperCount];

		uint32_t DebugUtilsExtension = 0;
#if defined(RUN_DEBUG)
		DebugUtilsExtension = 1;
#endif

		for (uint32_t i = 0; i < HelperCount - (SwapchainColourSpaceExtension + DebugUtilsExtension); i++) {
			Extensions[i] = RequiredExtensions[i];
		}
		if (SwapchainColourSpaceExtension) Extensions[HelperCount - 2] = VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME;
		if(DebugUtilsExtension) Extensions[HelperCount - 1] = "VK_EXT_debug_utils";

		const VkInstanceCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#if defined(RUN_DEBUG)
			.pNext = &DebugInfo,
#elif defined(RUN_PRODUCT)
			.pNext = nullptr,
#endif
			.flags = 0,
			.pApplicationInfo = &AppInfo,
#if defined(RUN_DEBUG)
			.enabledLayerCount = 1,
			.ppEnabledLayerNames = &Validation,
#elif defined(RUN_PRODUCT)
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
#endif
			.enabledExtensionCount = HelperCount,
			.ppEnabledExtensionNames = Extensions
		};
		CHECK(vkCreateInstance(&CreateInfo, nullptr, &g_instance))
		delete[] Extensions;
		volkLoadInstance(g_instance);
	}

	VkPhysicalDevice Physical = VK_NULL_HANDLE;

	{
		{
			CHECK(vkEnumeratePhysicalDevices(g_instance, &HelperCount, nullptr))
			auto Devices = new VkPhysicalDevice[HelperCount];
			CHECK(vkEnumeratePhysicalDevices(g_instance, &HelperCount, Devices))
			VkPhysicalDeviceProperties Props = { 0 };
			for(uint32_t i = 0; i < HelperCount; i++) {
				vkGetPhysicalDeviceProperties(Devices[i], &Props);
				if(Props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
					Physical = Devices[i];
					break;
				} else if(Props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) Physical = Devices[i];
			}
			delete[] Devices;
		}
		
		VkPhysicalDeviceMemoryProperties Properties = { 0 };
		vkGetPhysicalDeviceMemoryProperties(Physical, &Properties);
		for(uint32_t i = 0; i < Properties.memoryTypeCount; i++) {
			if(Properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
				m_localMemoryIndex = i;
				break;
			}
		}
		for(uint32_t i = 0; i < Properties.memoryTypeCount; i++) {
			if(Properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
				m_sharedMemoryIndex = i;
				break;
			}
		}
	}

	{
		{
			vkGetPhysicalDeviceQueueFamilyProperties(Physical, &HelperCount, nullptr);
			auto Properties = new VkQueueFamilyProperties[HelperCount];
			vkGetPhysicalDeviceQueueFamilyProperties(Physical, &HelperCount, Properties);

			for(uint32_t i = 0; i < HelperCount; i++) {
				if(Properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					m_graphicsQueueFamilyIndex = i;
					m_multiThreadedGraphics = Properties[i].queueCount > 3 ? true : false;
					break;
				}
			}
			delete[] Properties;
		}
		
		VkPhysicalDeviceVulkan13Features Features13 = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
			.pNext = nullptr
		};
		VkPhysicalDeviceFeatures2 FeaturesExtent = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
			.pNext = &Features13
		};
		vkGetPhysicalDeviceFeatures2(Physical, &FeaturesExtent);

		float Priorities[GRAPHICS_THREAD_COUNT] = { 0 };
		{
			float Value = 1;
			for(uint32_t i = 0; i < GRAPHICS_THREAD_COUNT; i++) {
				Priorities[i] = Value;
				Value -= 0.1;
			}
		}
		
		const VkDeviceQueueCreateInfo QueueInfos[] = {
			{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.queueFamilyIndex = m_graphicsQueueFamilyIndex,
				.queueCount = static_cast<uint32_t>(m_multiThreadedGraphics ? GRAPHICS_THREAD_COUNT : 1),
				.pQueuePriorities = Priorities
			}
		};
		const char* Extension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
		const VkDeviceCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = &FeaturesExtent,
			.flags = 0,
			.queueCreateInfoCount = SIZE_OF(QueueInfos),
			.pQueueCreateInfos = QueueInfos,
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
			.enabledExtensionCount = 1,
			.ppEnabledExtensionNames = &Extension,
			.pEnabledFeatures = nullptr
		};
		CHECK(vkCreateDevice(Physical, &CreateInfo, nullptr, &m_logical))
		volkLoadDevice(m_logical);
	}
	
	build_commandThread(m_graphicsQueueFamilyIndex, 0, &m_deferredRenderingCommandQueue, &m_deferredRenderingCommandPool, &m_deferredRenderingCommandSet);
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		if(m_multiThreadedGraphics) build_commandThread(m_graphicsQueueFamilyIndex, i, &m_shadowMappingCommandQueues[i], &m_shadowMappingCommandPools[i], &m_shadowMappingCommandSets[i]);
		else build_commandThread(m_graphicsQueueFamilyIndex, 0, &m_deferredRenderingCommandQueue, &m_shadowMappingCommandPools[i], &m_shadowMappingCommandSets[i]);
	}
	
	CHECK(glfwCreateWindowSurface(g_instance, in_window, nullptr, &m_surface))
	m_surfaceExtent = in_extent;
	{
		VkSurfaceCapabilitiesKHR Caps = { 0 };
		CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Physical, m_surface, &Caps))
		m_surfaceFrameCount = Caps.minImageCount + 1;
		CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(Physical, m_surface, &HelperCount, nullptr))
		auto Formats = new VkSurfaceFormatKHR[HelperCount];
		CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(Physical, m_surface, &HelperCount, Formats))
		for(int i = 0; i < HelperCount; i++) {
			m_surfaceFormat = Formats[i].format > m_surfaceFormat ? Formats[i].format : m_surfaceFormat;
			m_surfaceColourSpace = Formats[i].colorSpace > m_surfaceColourSpace ? Formats[i].colorSpace : m_surfaceColourSpace;
		}
		delete[] Formats;
	}
}

void GPUFixedContext::ruin_device(void) {
	ruin_commandThread(&m_deferredRenderingCommandPool, &m_deferredRenderingCommandSet);
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		ruin_commandThread(&m_shadowMappingCommandPools[i], &m_shadowMappingCommandSets[i]);
	}
	vkDestroySurfaceKHR(g_instance, m_surface, nullptr);
	vkDestroyDevice(m_logical, nullptr);
	vkDestroyInstance(g_instance, nullptr);
	volkFinalize();
}



#endif
