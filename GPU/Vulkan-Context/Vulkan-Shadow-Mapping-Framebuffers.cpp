#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static GPULocalTexture g_textures[CASCADED_SHADOW_MAP_COUNT][MAX_LIGHT_COUNT] = { { VK_NULL_HANDLE } };



void GPUFixedContext::build_shadowMappingFramebuffers(void) {
	VkExtent3D Extent = SHADOW_MAP_EXTENT;
	uint32_t Divisor = 1;

	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		Extent.width /= Divisor;
		Extent.height /= Divisor;
		for(uint32_t j = 0; j < MAX_LIGHT_COUNT; j++) {
			build_localTexture(&g_textures[i][j], nullptr, VK_FORMAT_D32_SFLOAT, Extent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
		}
		Divisor++;
	}
	
	Extent = SHADOW_MAP_EXTENT;
	VkFramebufferCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.renderPass = m_shadowMappingPass,
		.attachmentCount = 1,
		.pAttachments = nullptr,
		.width = Extent.width,
		.height = Extent.height,
		.layers = 1
	};
	
	Divisor = 1;
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		CreateInfo.width /= Divisor;
		CreateInfo.height /= Divisor;
		for(uint32_t j = 0; j < MAX_LIGHT_COUNT; j++) {
			CreateInfo.pAttachments = &g_textures[i][j].view;
			CHECK(vkCreateFramebuffer(m_logical, &CreateInfo, nullptr, &m_shadowMappingFramebuffers[i][j]))
		}
		Divisor++;
	}
}

void GPUFixedContext::ruin_shadowMappingFramebuffers(void) {
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		for(uint32_t j = 0; j < MAX_LIGHT_COUNT; j++) {
			vkDestroyFramebuffer(m_logical, m_shadowMappingFramebuffers[i][j], nullptr);
			ruin_localTexture(&g_textures[i][j]);
		}
	}
}



#endif
