#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_surface(void) {
	CHECK(glfwCreateWindowSurface(m_context, m_surfaceWindow, nullptr, &m_surface))

	VkSurfaceCapabilitiesKHR Caps = { 0 };
	CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical, m_surface, &Caps))
	m_surfaceFrameCount = Caps.minImageCount + 1;
	uint32_t FormatCount = 0;
	CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical, m_surface, &FormatCount, nullptr))
	auto Formats = new VkSurfaceFormatKHR[FormatCount];
	CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical, m_surface, &FormatCount, Formats))
	for(uint32_t i = 0; i < FormatCount; i++) {
		m_surfaceFormat = Formats[i].format > m_surfaceFormat ? Formats[i].format : m_surfaceFormat;
		m_surfaceColourSpace = Formats[i].colorSpace > m_surfaceColourSpace ? Formats[i].colorSpace : m_surfaceColourSpace;
	}
	delete[] Formats;
}

void GPUFixedContext::ruin_surface(void) {
	vkDestroySurfaceKHR(m_context, m_surface, nullptr);
}



#endif
