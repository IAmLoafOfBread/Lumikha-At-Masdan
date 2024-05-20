#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



#define ATTACHMENT_COUNT 2

static GPULocalTexture g_depthTexture = { VK_NULL_HANDLE };



void GPUFixedContext::build_lightingFramebuffers(void) {
	build_localTexture(&g_depthTexture, nullptr, VK_FORMAT_D32_SFLOAT, m_surfaceExtent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

	{
		VkImageView Attachments[ATTACHMENT_COUNT] = { VK_NULL_HANDLE };
		Attachments[1] = g_depthTexture.view;

		const VkFramebufferCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderPass = m_lightingPass,
			.attachmentCount = ATTACHMENT_COUNT,
			.pAttachments = Attachments,
			.width = m_surfaceExtent.width,
			.height = m_surfaceExtent.height,
			.layers = 1
		};
		for (uint32_t i = 0; i < m_surfaceFrameCount; i++) {
			Attachments[0] = m_presentViews[i];
			CHECK(vkCreateFramebuffer(m_logical, &CreateInfo, nullptr, &m_lightingFramebuffers[i]))
		}
	}
}

void GPUFixedContext::ruin_lightingFramebuffers(void) {
	for(uint32_t i = 0; i < m_surfaceFrameCount; i++) vkDestroyFramebuffer(m_logical, m_lightingFramebuffers[i], nullptr);
	ruin_localTexture(&g_depthTexture);
}



#endif
