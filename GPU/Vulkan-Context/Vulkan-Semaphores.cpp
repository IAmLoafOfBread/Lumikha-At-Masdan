#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"


#define name(v) "/ LaM::"#v".sem"


void GPUFixedContext::build_semaphores(void) {
	const VkSemaphoreCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	};
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		CHECK(vkCreateSemaphore(m_logical, &CreateInfo, nullptr, &m_shadowMappingsFinishedSemaphores[i]))
	}
	CHECK(vkCreateSemaphore(m_logical, &CreateInfo, nullptr, &m_imageAvailableSemaphore))
	CHECK(vkCreateSemaphore(m_logical, &CreateInfo, nullptr, &m_lightViewingsFinishedSemaphore))
	CHECK(vkCreateSemaphore(m_logical, &CreateInfo, nullptr, &m_renderFinishedSemaphore))
	
	create_semaphore(&m_cameraSemaphore, name(m_cameraSemaphore));
	create_semaphore(&m_instancesSemaphore, name(m_instancesSemaphore));
	create_semaphore(&m_lightsSemaphore, name(m_lightsSemaphore));
}

void GPUFixedContext::ruin_semaphores(void) {
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		vkDestroySemaphore(m_logical, m_shadowMappingsFinishedSemaphores[i], nullptr);
	}
	vkDestroySemaphore(m_logical, m_imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(m_logical, m_lightViewingsFinishedSemaphore, nullptr);
	vkDestroySemaphore(m_logical, m_renderFinishedSemaphore, nullptr);
	
	destroy_semaphore(m_cameraSemaphore, name(m_cameraSemaphore));
	destroy_semaphore(m_instancesSemaphore, name(m_instancesSemaphore));
	destroy_semaphore(m_lightsSemaphore, name(m_lightsSemaphore));
}



#endif
