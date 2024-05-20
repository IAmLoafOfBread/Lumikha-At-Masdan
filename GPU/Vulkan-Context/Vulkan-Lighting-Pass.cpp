#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



#define ATTACHMENT_COUNT 2



void GPUFixedContext::build_lightingPass(void) {
	{
		VkAttachmentDescription Descriptions[ATTACHMENT_COUNT] = { { 0 } };
		{
			const VkFormat Formats[ATTACHMENT_COUNT] = {m_surfaceFormat, VK_FORMAT_D32_SFLOAT};
			for (uint32_t i = 0; i < ATTACHMENT_COUNT; i++) {
				Descriptions[i].flags = 0;
				Descriptions[i].format = Formats[i];
				Descriptions[i].samples = VK_SAMPLE_COUNT_1_BIT;
				Descriptions[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				Descriptions[i].storeOp = Formats[i] != VK_FORMAT_D32_SFLOAT ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
				Descriptions[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				Descriptions[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				Descriptions[i].initialLayout = Formats[i] != VK_FORMAT_D32_SFLOAT ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				Descriptions[i].finalLayout = Formats[i] != VK_FORMAT_D32_SFLOAT ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}
		}

		const VkAttachmentReference References[ATTACHMENT_COUNT] = {
			{
				.attachment = 0,
				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			},
			{
				.attachment = 1,
				.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			},
		};

		VkSubpassDescription Subpass = {
			.flags = 0,
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.inputAttachmentCount = 0,
			.pInputAttachments = nullptr,
			.colorAttachmentCount = 1,
			.pColorAttachments = References,
			.pResolveAttachments = nullptr,
			.pDepthStencilAttachment = &References[1],
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = nullptr
		};

		const VkRenderPassCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.attachmentCount = ATTACHMENT_COUNT,
			.pAttachments = Descriptions,
			.subpassCount = 1,
			.pSubpasses = &Subpass,
			.dependencyCount = 0,
			.pDependencies = nullptr
		};
		CHECK(vkCreateRenderPass(m_logical, &CreateInfo, nullptr, &m_lightingPass))
	}
}

void GPUFixedContext::ruin_lightingPass(void) {
	vkDestroyRenderPass(m_logical, m_lightingPass, nullptr);
}



#endif
