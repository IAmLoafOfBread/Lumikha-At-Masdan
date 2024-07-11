#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_semaphores(void) {
	const VkSemaphoreCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	};

	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		CHECK(vkCreateSemaphore(m_logical, &CreateInfo, nullptr, &m_lightViewingsFinishedSemaphores[i]))
		CHECK(vkCreateSemaphore(m_logical, &CreateInfo, nullptr, &m_shadowMappingsFinishedSemaphores[i]))
	}
	CHECK(vkCreateSemaphore(m_logical, &CreateInfo, nullptr, &m_geometryFinishedSemaphore))
	CHECK(vkCreateSemaphore(m_logical, &CreateInfo, nullptr, &m_lightingFinishedSemaphores[0]))
	CHECK(vkCreateSemaphore(m_logical, &CreateInfo, nullptr, &m_lightingFinishedSemaphores[1]))
}

void GPUFixedContext::ruin_semaphores(void) {
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		vkDestroySemaphore(m_logical, m_lightViewingsFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_logical, m_shadowMappingsFinishedSemaphores[i], nullptr);
	}
	vkDestroySemaphore(m_logical, m_geometryFinishedSemaphore, nullptr);
	vkDestroySemaphore(m_logical, m_lightingFinishedSemaphores[0], nullptr);
	vkDestroySemaphore(m_logical, m_lightingFinishedSemaphores[1], nullptr);
}



#endif
