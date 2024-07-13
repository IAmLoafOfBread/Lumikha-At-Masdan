#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkFence g_fences[CASCADED_SHADOW_MAP_COUNT + 1] = { VK_NULL_HANDLE };



void GPUFixedContext::initialize_instanceUpdateData(void) {
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		g_fences[i] = m_shadowMappingsFinishedFences[i];
	}
	g_fences[CASCADED_SHADOW_MAP_COUNT] = m_geometryFinishedFence;
}

void GPUFixedContext::add_instance(uint32_t in_type, Instance* in_instance) {
	wait_semaphore(m_instancesSemaphore);
	CHECK(vkWaitForFences(m_logical, LENGTH_OF(g_fences), g_fences, VK_TRUE, UINT64_MAX))
	m_instances[m_indirectCommands[in_type].instanceCount] = *in_instance;
	m_indirectCommands[in_type].instanceCount++;
	signal_semaphore(m_instancesSemaphore);
}

void GPUFixedContext::rid_instance(uint32_t in_type, uint32_t in_index) {
	wait_semaphore(m_instancesSemaphore);
	CHECK(vkWaitForFences(m_logical, LENGTH_OF(g_fences), g_fences, VK_TRUE, UINT64_MAX))
	m_indirectCommands[in_type].instanceCount--;
	m_instances[in_index] = m_instances[m_indirectCommands[in_type].instanceCount];
	signal_semaphore(m_instancesSemaphore);
}



#endif
