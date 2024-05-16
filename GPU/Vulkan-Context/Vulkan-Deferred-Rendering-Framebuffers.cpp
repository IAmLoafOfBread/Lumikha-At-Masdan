#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_deferredRenderingFramebuffers(void) {
	{
		const VkFormat Formats[DEFERRED_RENDERING_ATTACHMENT_COUNT] = DEFERRED_RENDERING_ATTACHMENT_FORMATS;
		for(uint32_t i = 0; i < GEOMETRY_PASS_OUTPUT_COUNT; i++) {
			if(i != DEFERRED_RENDERING_ATTACHMENT_DEPTH_INDEX) {
				build_localTexture(&m_geometryOutputAttachments[i], nullptr, Formats[i], m_surfaceExtent, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
			} else {
				build_localTexture(&m_geometryOutputAttachments[i], nullptr, Formats[i], m_surfaceExtent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
			}
		}
	}
	
	{
		VkImageView Attachs[DEFERRED_RENDERING_ATTACHMENT_COUNT] = { VK_NULL_HANDLE };
		for(uint32_t i = 0; i < GEOMETRY_PASS_OUTPUT_COUNT; i++) Attachs[i] = m_geometryOutputAttachments[i].view;
		
		VkFramebufferCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderPass = m_deferredRenderingPass,
			.attachmentCount = DEFERRED_RENDERING_ATTACHMENT_COUNT,
			.pAttachments = Attachs,
			.width = m_surfaceExtent.width,
			.height = m_surfaceExtent.height,
			.layers = 1
		};
		
		m_deferredRenderingFramebuffers = new GPUFramebuffer[m_surfaceFrameCount];
		for(uint32_t i = 0; i < m_surfaceFrameCount; i++) {
			Attachs[DEFERRED_RENDERING_ATTACHMENT_PRESENT_INDEX] = m_presentViews[i];
			CHECK(vkCreateFramebuffer(m_logical, &CreateInfo, nullptr, &m_deferredRenderingFramebuffers[i]))
		}
	}
}

void GPUFixedContext::ruin_deferredRenderingFramebuffers(void) {
	for(uint32_t i = 0; i < m_surfaceFrameCount; i++) {
		vkDestroyFramebuffer(m_logical, m_deferredRenderingFramebuffers[i], nullptr);
	}
	delete[] m_deferredRenderingFramebuffers;
	for(uint32_t i = 0; i < GEOMETRY_PASS_OUTPUT_COUNT; i++) ruin_localTexture(&m_geometryOutputAttachments[i]);
}



#endif
