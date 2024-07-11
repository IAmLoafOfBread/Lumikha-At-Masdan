#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::obtain_physical(void) {
	{
		uint32_t DeviceCount = 0;
		CHECK(vkEnumeratePhysicalDevices(m_context, &DeviceCount, nullptr))
		auto Devices = new VkPhysicalDevice[DeviceCount];
		CHECK(vkEnumeratePhysicalDevices(m_context, &DeviceCount, Devices))
		VkPhysicalDeviceProperties Properties = { 0 };
		for (uint32_t i = 0; i < DeviceCount; i++) {
			vkGetPhysicalDeviceProperties(Devices[i], &Properties);
			if (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				m_physical = Devices[i];
				break;
			}
			else if (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) m_physical = Devices[i];
		}
		delete[] Devices;
	}
	{
		VkPhysicalDeviceMemoryProperties Properties = { 0 };
		vkGetPhysicalDeviceMemoryProperties(m_physical, &Properties);
		for (uint32_t i = 0; i < Properties.memoryTypeCount; i++) {
			if (Properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
				m_localMemoryIndex = i;
				break;
			}
		}
		for (uint32_t i = 0; i < Properties.memoryTypeCount; i++) {
			if (Properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
				m_sharedMemoryIndex = i;
				break;
			}
		}
	}

	uint32_t QueueCount = 0;
	{
		uint32_t FamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_physical, &FamilyCount, nullptr);
		auto Properties = new VkQueueFamilyProperties[FamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(m_physical, &FamilyCount, Properties);

		for (uint32_t i = 0; i < FamilyCount; i++) {
			if (Properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				m_graphicsQueueFamilyIndex = i;
				m_multiThreadedGraphics = Properties[i].queueCount > CASCADED_SHADOW_MAP_COUNT ? true : false;
				QueueCount = Properties[i].queueCount;
				break;
			}
		}
		for (uint32_t i = 0; i < FamilyCount; i++) {
			if (Properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT && m_graphicsQueueFamilyIndex != i) {
				m_computeQueueFamilyIndex = i;
				break;
			}
		}
		delete[] Properties;
	}

	m_queueFamilyCount = 2;
	if(m_computeQueueFamilyIndex == NULL_VALUE) {
		m_queueFamilyCount = 1;
		m_computeQueueFamilyIndex = m_graphicsQueueFamilyIndex;
		if (!(QueueCount > CASCADED_SHADOW_MAP_COUNT + 1)) {
			m_computeQueueIndex = NULL_VALUE;
		}
	}
}



#endif
