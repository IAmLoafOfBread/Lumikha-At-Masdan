#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::add_instance(uint32_t in_type, Instance* in_instance) {
	wait_semaphore(m_instancesSemaphore);
	m_instances[m_indirectCommands[in_type].instanceCount] = *in_instance;
	m_indirectCommands[in_type].instanceCount++;
	signal_semaphore(m_instancesSemaphore);
}

void GPUFixedContext::rid_instance(uint32_t in_type, uint32_t in_index) {
	wait_semaphore(m_instancesSemaphore);
	m_indirectCommands[in_type].instanceCount--;
	m_instances[in_index] = m_instances[m_indirectCommands[in_type].instanceCount];
	signal_semaphore(m_instancesSemaphore);
}

void GPUFixedContext::add_light(Light* in_light) {
	wait_semaphore(m_lightsSemaphore);
	m_lights[m_lightCount] = *in_light;
	m_lightCount++;
	signal_semaphore(m_lightsSemaphore);
}

void GPUFixedContext::rid_light(uint32_t in_index) {
	wait_semaphore(m_lightsSemaphore);
	m_lightCount--;
	m_lights[in_index] = m_lights[m_lightCount];
	signal_semaphore(m_lightsSemaphore);
}



#endif
