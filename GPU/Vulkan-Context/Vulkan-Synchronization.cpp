#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_synchronizations(void) {
	{
		const VkFenceCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};
		CHECK(vkCreateFence(m_logical, &CreateInfo, nullptr, &m_lightViewingsFinishedFence))
		for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
			CHECK(vkCreateFence(m_logical, &CreateInfo, nullptr, &m_shadowMappingsFinishedFences[i]))
		}
		CHECK(vkCreateFence(m_logical, &CreateInfo, nullptr, &m_geometryFinishedFence))
		CHECK(vkCreateFence(m_logical, &CreateInfo, nullptr, &m_lightingFinishedFence))
		CHECK(vkCreateFence(m_logical, &CreateInfo, nullptr, &m_presentFinishedFence))
	}

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

void GPUFixedContext::ruin_synchronizations(void) {
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		vkDestroySemaphore(m_logical, m_lightViewingsFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_logical, m_shadowMappingsFinishedSemaphores[i], nullptr);
	}
	vkDestroySemaphore(m_logical, m_geometryFinishedSemaphore, nullptr);
	vkDestroySemaphore(m_logical, m_lightingFinishedSemaphores[0], nullptr);
	vkDestroySemaphore(m_logical, m_lightingFinishedSemaphores[1], nullptr);

	vkDestroyFence(m_logical, m_lightViewingsFinishedFence, nullptr);
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		vkDestroyFence(m_logical, m_shadowMappingsFinishedFences[i], nullptr);
	}
	vkDestroyFence(m_logical, m_geometryFinishedFence, nullptr);
	vkDestroyFence(m_logical, m_lightingFinishedFence, nullptr);
	vkDestroyFence(m_logical, m_presentFinishedFence, nullptr);
}



#endif
