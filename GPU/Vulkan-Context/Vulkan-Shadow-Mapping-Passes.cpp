#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_shadowMappingPasses(void) {
	const VkAttachmentDescription Description = {
		.flags = 0,
		.format = VK_FORMAT_D32_SFLOAT,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		.finalLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
	};
	const VkAttachmentReference Reference = {
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};
	const VkSubpassDescription Pass = {
		.flags = 0,
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.inputAttachmentCount = 0,
		.pInputAttachments = nullptr,
		.colorAttachmentCount = 0,
		.pColorAttachments = nullptr,
		.pResolveAttachments = nullptr,
		.pDepthStencilAttachment = &Reference,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = nullptr
	};
	const VkRenderPassCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.attachmentCount = 1,
		.pAttachments = &Description,
		.subpassCount = 1,
		.pSubpasses = &Pass,
		.dependencyCount = 0,
		.pDependencies = nullptr
	};
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		CHECK(vkCreateRenderPass(m_logical, &CreateInfo, nullptr, &m_shadowMappingPasses[i]))
	}
}

void GPUFixedContext::ruin_shadowMappingPasses(void) {
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) vkDestroyRenderPass(m_logical, m_shadowMappingPasses[i], nullptr);
}



#endif
