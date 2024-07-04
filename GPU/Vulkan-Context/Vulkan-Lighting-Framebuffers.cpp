#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_lightingFramebuffers(void) {
	m_lightingFramebuffers = new VkFramebuffer[m_surfaceFrameCount];

	VkFramebufferCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.renderPass = m_lightingPass,
		.attachmentCount = 1,
		.pAttachments = nullptr,
		.width = m_surfaceExtent.width,
		.height = m_surfaceExtent.height,
		.layers = 1
	};
	for (uint32_t i = 0; i < m_surfaceFrameCount; i++) {
		CreateInfo.pAttachments = &m_presentViews[i];
		CHECK(vkCreateFramebuffer(m_logical, &CreateInfo, nullptr, &m_lightingFramebuffers[i]))
	}
}

void GPUFixedContext::ruin_lightingFramebuffers(void) {
	for(uint32_t i = 0; i < m_surfaceFrameCount; i++) vkDestroyFramebuffer(m_logical, m_lightingFramebuffers[i], nullptr);
	delete[] m_lightingFramebuffers;
}



#endif
