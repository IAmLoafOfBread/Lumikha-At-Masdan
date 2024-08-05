#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_postProcessingFramebuffers(void) {
	m_postProcessingFramebuffers = new VkFramebuffer[m_surfaceFrameCount];
	
	VkFramebufferCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.renderPass = m_postProcessingPass,
		.attachmentCount = 1,
		.pAttachments = nullptr,
		.width = m_surfaceExtent.width,
		.height = m_surfaceExtent.height,
		.layers = 1
	};
	for(uint32_t i = 0; i < m_surfaceFrameCount; i++) {
		CreateInfo.pAttachments = &m_presentViews[i];
		CHECK(vkCreateFramebuffer(m_logical, &CreateInfo, nullptr, &m_postProcessingFramebuffers[i]))
	}
}

void GPUFixedContext::ruin_postProcessingFramebuffers(void) {
	for(uint32_t i = 0; i < m_surfaceFrameCount; i++) vkDestroyFramebuffer(m_logical, m_postProcessingFramebuffers[i], nullptr);
	delete[] m_postProcessingFramebuffers;
}



#endif
