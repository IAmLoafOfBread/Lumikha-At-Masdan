#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkFence g_fences[1 + CASCADED_SHADOW_MAP_COUNT + 1] = { VK_NULL_HANDLE };



void GPUFixedContext::initialize_lightUpdateData(void) {
	g_fences[0] = m_lightViewingsFinishedFence;
	for (uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		g_fences[i + 1] = m_shadowMappingsFinishedFences[i];
	}
	g_fences[CASCADED_SHADOW_MAP_COUNT + 1] = m_lightingFinishedFence;
}

void GPUFixedContext::add_light(Light* in_light) {
	wait_semaphore(m_lightsSemaphore);
	CHECK(vkWaitForFences(m_logical, LENGTH_OF(g_fences), g_fences, VK_TRUE, UINT64_MAX))
	m_lights[m_lightCount] = *in_light;
	m_lightCount++;
	signal_semaphore(m_lightsSemaphore);
}

void GPUFixedContext::rid_light(uint32_t in_index) {
	wait_semaphore(m_lightsSemaphore);
	CHECK(vkWaitForFences(m_logical, LENGTH_OF(g_fences), g_fences, VK_TRUE, UINT64_MAX))
	m_lightCount--;
	m_lights[in_index] = m_lights[m_lightCount];
	signal_semaphore(m_lightsSemaphore);
}



#endif
