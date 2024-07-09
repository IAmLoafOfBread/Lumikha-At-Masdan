#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



#define ATTACHMENT_COUNT (GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT + 1)



void GPUFixedContext::build_geometryFramebuffer(void) {
	{
		const VkFormat Formats[ATTACHMENT_COUNT] = {GEOMETRY_PASS_COLOUR_ATTACHMENT_FORMATS, VK_FORMAT_D32_SFLOAT};
		for(uint32_t i = 0; i < ATTACHMENT_COUNT; i++) {
			const VkImageUsageFlags Flags = Formats[i] != VK_FORMAT_D32_SFLOAT ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT : VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			const VkImageLayout Layout = Formats[i] != VK_FORMAT_D32_SFLOAT ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
			build_localTexture(&m_geometryTextures[i], nullptr, Formats[i], m_surfaceExtent, Flags, Layout);
		}
	}
	
	{
		VkImageView Attachments[ATTACHMENT_COUNT] = { VK_NULL_HANDLE };
		for (uint32_t i = 0; i < ATTACHMENT_COUNT; i++) {
			Attachments[i] = m_geometryTextures[i].view;
		}
		
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
	for(uint32_t i = 0; i < ATTACHMENT_COUNT; i++) ruin_localTexture(&m_geometryTextures[i]);
}



#endif
