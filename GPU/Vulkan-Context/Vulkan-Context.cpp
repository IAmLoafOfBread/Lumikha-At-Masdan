#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"
#include <string.h>



#if defined(RUN_DEBUG)
static VKAPI_ATTR VkBool32 VKAPI_CALL g_debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	printf("Validation Layer: %s\n", pCallbackData->pMessage);
	return VK_FALSE;
}
#endif



void GPUFixedContext::build_context(void) {
	volkInitialize();

	const VkApplicationInfo AppInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = "Temper",
		.applicationVersion = VK_VERSION_1_0,
		.pEngineName = "TemperLogic",
		.engineVersion = VK_VERSION_1_0,
		.apiVersion = VK_API_VERSION_1_2
	};

#if defined(RUN_DEBUG)
	const char* Layers[] = { "VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_api_dump" };
	const VkDebugUtilsMessengerCreateInfoEXT DebugInfo = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext = nullptr,
		.flags = 0,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
		.pfnUserCallback = &g_debugCallback
	};
#endif

	uint32_t ExtensionCount = 0;
	uint32_t SwapchainColourSpaceExtension = 0;
	{
		CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr))
			auto ExtensionProperties = new VkExtensionProperties[ExtensionCount];
		CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, ExtensionProperties))
			for (uint32_t i = 0; i < ExtensionCount; i++) if (!strcmp(ExtensionProperties[i].extensionName, VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME)) SwapchainColourSpaceExtension = 1;
		delete[] ExtensionProperties;
	}

	const char** RequiredExtensions = glfwGetRequiredInstanceExtensions(&ExtensionCount);
	ExtensionCount += SwapchainColourSpaceExtension;
#if defined(RUN_DEBUG)
	ExtensionCount++;
#endif
	auto Extensions = new const char* [ExtensionCount];

	uint32_t DebugUtilsExtension = 0;
#if defined(RUN_DEBUG)
	DebugUtilsExtension = 1;
#endif

	for (uint32_t i = 0; i < ExtensionCount - (SwapchainColourSpaceExtension + DebugUtilsExtension); i++) {
		Extensions[i] = RequiredExtensions[i];
	}
	if (SwapchainColourSpaceExtension) Extensions[ExtensionCount - 2] = VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME;
	if (DebugUtilsExtension) Extensions[ExtensionCount - 1] = "VK_EXT_debug_utils";

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
		.ppEnabledLayerNames = Layers,
#elif defined(RUN_PRODUCT)
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
#endif
		.enabledExtensionCount = ExtensionCount,
		.ppEnabledExtensionNames = Extensions
	};
	CHECK(vkCreateInstance(&CreateInfo, nullptr, &m_context))
	delete[] Extensions;
	volkLoadInstance(m_context);
}

void GPUFixedContext::ruin_context(void) {
	vkDestroyInstance(m_context, nullptr);
	volkFinalize();
}



#endif
