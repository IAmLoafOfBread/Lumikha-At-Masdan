#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_shadowMappingFramebuffers(void) {
	const VkExtent3D Extent = SHADOW_MAP_EXTENT;
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		for(uint32_t j = 0; j < MAX_LIGHT_COUNT; j++) {
			build_localTexture(&m_shadowMappingAttachments[i][j], nullptr, VK_FORMAT_D32_SFLOAT, Extent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
		}
	}
	
	VkFramebufferCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.renderPass = VK_NULL_HANDLE,
		.attachmentCount = 1,
		.pAttachments = nullptr,
		.width = Extent.width,
		.height = Extent.height,
		.layers = 1
	};
	
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		CreateInfo.renderPass = m_shadowMappingPasses[i];
		for(uint32_t j = 0; j < MAX_LIGHT_COUNT; j++) {
			CreateInfo.pAttachments = &m_shadowMappingAttachments[i][j].view;
			CHECK(vkCreateFramebuffer(m_logical, &CreateInfo, nullptr, &m_shadowMappingFramebuffers[i][j]))
		}
	}
}

void GPUFixedContext::ruin_shadowMappingFramebuffers(void) {
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		for(uint32_t j = 0; j < MAX_LIGHT_COUNT; j++) {
			vkDestroyFramebuffer(m_logical, m_shadowMappingFramebuffers[i][j], nullptr);
			ruin_localTexture(&m_shadowMappingAttachments[i][j]);
		}
	}
}



#endif
