#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_commandThreads(void) {
	uint32_t ComputeQueueOffset = 0;
	if(m_computeQueueFamilyIndex == m_graphicsQueueFamilyIndex && m_computeQueueIndex == 0) {
		ComputeQueueOffset = 1;
	}
	vkGetDeviceQueue(m_logical, m_computeQueueFamilyIndex, m_computeQueueIndex - ComputeQueueOffset, &m_lightViewingCommandQueue);
	build_commandThread(m_computeQueueFamilyIndex, &m_lightViewingCommandPool, &m_lightViewingCommandSet);

	for (uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		const uint32_t Index = m_multiThreadedGraphics ? i + 1 + ComputeQueueOffset : 0;
		vkGetDeviceQueue(m_logical, m_graphicsQueueFamilyIndex, Index, &m_shadowMappingCommandQueues[i]);
		build_commandThread(m_graphicsQueueFamilyIndex, &m_shadowMappingCommandPools[i], &m_shadowMappingCommandSets[i]);
	}

	vkGetDeviceQueue(m_logical, m_graphicsQueueFamilyIndex, 0, &m_deferredRenderingCommandQueue);
	build_commandThread(m_graphicsQueueFamilyIndex, &m_geometryCommandPool, &m_geometryCommandSet);
	build_commandThread(m_graphicsQueueFamilyIndex, &m_lightingCommandPool, &m_lightingCommandSet);
	build_commandThread(m_graphicsQueueFamilyIndex, &m_presentCommandPool, &m_presentCommandSet);
}

void GPUFixedContext::ruin_commandThreads(void) {
	ruin_commandThread(&m_lightViewingCommandPool, &m_lightViewingCommandSet);
	for (uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		ruin_commandThread(&m_shadowMappingCommandPools[i], &m_shadowMappingCommandSets[i]);
	}
	ruin_commandThread(&m_geometryCommandPool, &m_geometryCommandSet);
	ruin_commandThread(&m_lightingCommandPool, &m_lightingCommandSet);
	ruin_commandThread(&m_presentCommandPool, &m_presentCommandSet);
}



#endif
