#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_lightingFramebuffer(void) {
	build_localTexture(&m_lightingTexture, nullptr, VK_FORMAT_R32G32B32A32_SFLOAT, m_surfaceExtent, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	const VkFramebufferCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.renderPass = m_lightingPass,
		.attachmentCount = 1,
		.pAttachments = &m_lightingTexture.view,
		.width = m_surfaceExtent.width,
		.height = m_surfaceExtent.height,
		.layers = 1
	};
	CHECK(vkCreateFramebuffer(m_logical, &CreateInfo, nullptr, &m_lightingFramebuffer))
}

void GPUFixedContext::ruin_lightingFramebuffer(void) {
	vkDestroyFramebuffer(m_logical, m_lightingFramebuffer, nullptr);
	ruin_localTexture(&m_lightingTexture);
}



#endif
