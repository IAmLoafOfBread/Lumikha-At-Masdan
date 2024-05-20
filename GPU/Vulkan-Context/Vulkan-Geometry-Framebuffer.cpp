#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



#define ATTACHMENT_COUNT (GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT + 1)

static GPULocalTexture g_textures[ATTACHMENT_COUNT] = { { VK_NULL_HANDLE } };



void GPUFixedContext::build_geometryFramebuffer(void) {
	{
		const VkFormat Formats[ATTACHMENT_COUNT] = {GEOMETRY_PASS_COLOUR_ATTACHMENT_FORMATS, VK_FORMAT_D32_SFLOAT};
		for(uint32_t i = 0; i < ATTACHMENT_COUNT; i++) {
			const VkImageUsageFlags Flags = Formats[i] != VK_FORMAT_D32_SFLOAT ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT : VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			build_localTexture(&g_textures[i], nullptr, Formats[i], m_surfaceExtent, Flags);
		}
	}
	
	{
		VkImageView Attachments[ATTACHMENT_COUNT] = { VK_NULL_HANDLE };
		for (uint32_t i = 0; i < GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT; i++) {
			m_geometryViews[i] = g_textures[i].view;
			Attachments[i] = g_textures[i].view;
		}
		Attachments[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT] = g_textures[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT].view;
		
		const VkFramebufferCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderPass = m_geometryPass,
			.attachmentCount = ATTACHMENT_COUNT,
			.pAttachments = Attachments,
			.width = m_surfaceExtent.width,
			.height = m_surfaceExtent.height,
			.layers = 1
		};
		CHECK(vkCreateFramebuffer(m_logical, &CreateInfo, nullptr, &m_geometryFramebuffer))
	}
}

void GPUFixedContext::ruin_geometryFramebuffer(void) {
	vkDestroyFramebuffer(m_logical, m_geometryFramebuffer, nullptr);
	for(uint32_t i = 0; i < ATTACHMENT_COUNT; i++) ruin_localTexture(&g_textures[i]);
}



#endif
