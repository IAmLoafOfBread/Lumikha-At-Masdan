#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_geometryPass(void) {
	VkAttachmentDescription Descriptions[GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT] = { { 0 } };
	{
		const VkFormat Formats[GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT] = {GEOMETRY_PASS_COLOUR_ATTACHMENT_FORMATS, VK_FORMAT_D32_SFLOAT};
		for(uint32_t i = 0; i < GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT; i++) {
			Descriptions[i].flags = 0;
			Descriptions[i].format = Formats[i];
			Descriptions[i].samples = VK_SAMPLE_COUNT_1_BIT;
			Descriptions[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			Descriptions[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			Descriptions[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			Descriptions[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			Descriptions[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			Descriptions[i].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
	}
	VkAttachmentReference References[GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT] = { { 0 } };
	for(uint32_t i = 0; i < GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT; i++) {
		References[i].attachment = i;
		References[i].layout = i != GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}
	const VkSubpassDescription Subpass = {
		.flags = 0,
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.inputAttachmentCount = 0,
		.pInputAttachments = nullptr,
		.colorAttachmentCount = GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT,
		.pColorAttachments = References,
		.pResolveAttachments = nullptr,
		.pDepthStencilAttachment = &References[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT],
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = nullptr
	};
	const VkSubpassDependency Dependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		.dependencyFlags = 0
	};
	const VkRenderPassCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.attachmentCount = GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT,
		.pAttachments = Descriptions,
		.subpassCount = 1,
		.pSubpasses = &Subpass,
		.dependencyCount = 1,
		.pDependencies = &Dependency
	};
	CHECK(vkCreateRenderPass(m_logical, &CreateInfo, nullptr, &m_geometryPass))
}

void GPUFixedContext::ruin_geometryPass(void) {
	vkDestroyRenderPass(m_logical, m_geometryPass, nullptr);
}



#endif
