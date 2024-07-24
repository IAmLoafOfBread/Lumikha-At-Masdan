#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_logical(void) {
	VkPhysicalDeviceVulkan12Features Features12 = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
		.pNext = nullptr,
		.runtimeDescriptorArray = VK_TRUE,
		.scalarBlockLayout = VK_TRUE,
		.bufferDeviceAddress = VK_TRUE
	};
	VkPhysicalDeviceVulkan11Features Features11 = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
		.pNext = &Features12,
		.variablePointersStorageBuffer = VK_TRUE,
		.variablePointers = VK_TRUE
	};
	VkPhysicalDeviceFeatures2 FeaturesExtent = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
		.pNext = &Features11
	};

	float Priorities[GRAPHICS_THREAD_COUNT + 1] = { 0 };
	{
		float Value = 1;
		for(uint32_t i = 0; i < GRAPHICS_THREAD_COUNT + 1; i++) {
			Priorities[i] = Value;
			Value -= 0.1f;
		}
	}
	
	uint32_t GraphicsQueueCount = m_multiThreadedGraphics ? GRAPHICS_THREAD_COUNT : 1;
	if(m_computeQueueFamilyIndex == m_graphicsQueueFamilyIndex) {
		GraphicsQueueCount += m_computeQueueIndex == NULL_VALUE ? 0 : 1;
	}
	const VkDeviceQueueCreateInfo QueueInfos[] = {
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = m_graphicsQueueFamilyIndex,
			.queueCount = GraphicsQueueCount,
			.pQueuePriorities = Priorities
		},
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = m_computeQueueFamilyIndex,
			.queueCount = 1,
			.pQueuePriorities = Priorities
		}
	};
	const char* Extension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
	const VkDeviceCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &FeaturesExtent,
		.flags = 0,
		.queueCreateInfoCount = m_queueFamilyCount,
		.pQueueCreateInfos = QueueInfos,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = 1,
		.ppEnabledExtensionNames = &Extension,
		.pEnabledFeatures = nullptr
	};
	CHECK(vkCreateDevice(m_physical, &CreateInfo, nullptr, &m_logical))
	volkLoadDevice(m_logical);
}

void GPUFixedContext::ruin_logical(void) {
	vkDestroyDevice(m_logical, nullptr);
}



#endif
