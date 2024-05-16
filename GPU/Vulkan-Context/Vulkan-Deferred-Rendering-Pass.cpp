#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_deferredRenderingPass(void) {
	{
		VkAttachmentDescription Descriptions[DEFERRED_RENDERING_ATTACHMENT_COUNT] = { { 0 } };
		{
			const VkFormat Formats[DEFERRED_RENDERING_ATTACHMENT_COUNT] = DEFERRED_RENDERING_ATTACHMENT_FORMATS;
			for(uint32_t i = 0; i < DEFERRED_RENDERING_ATTACHMENT_COUNT; i++) {
				Descriptions[i].flags = 0;
				Descriptions[i].format = Formats[i];
				Descriptions[i].samples = VK_SAMPLE_COUNT_1_BIT;
				Descriptions[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				Descriptions[i].storeOp = i == DEFERRED_RENDERING_ATTACHMENT_PRESENT_INDEX ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
				Descriptions[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				Descriptions[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				Descriptions[i].initialLayout = i == DEFERRED_RENDERING_ATTACHMENT_DEPTH_INDEX ? VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				if(i < DEFERRED_RENDERING_ATTACHMENT_DEPTH_INDEX) Descriptions[i].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				if(i == DEFERRED_RENDERING_ATTACHMENT_DEPTH_INDEX) Descriptions[i].finalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
				if(i == DEFERRED_RENDERING_ATTACHMENT_PRESENT_INDEX) Descriptions[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			}
		}
		
		VkAttachmentReference TotalReferences[] = { { 0 } };
		for(uint32_t i = 0; i < DEFERRED_RENDERING_ATTACHMENT_COUNT; i++) {
			TotalReferences[i].attachment = i;
			TotalReferences[i].layout = i == DEFERRED_RENDERING_ATTACHMENT_DEPTH_INDEX ? VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		
		VkAttachmentReference InputReferences[] = { { 0 } };
		for(uint32_t i = 0; i < LIGHTING_PASS_INPUT_COUNT; i++) {
			InputReferences[i].attachment = i;
			InputReferences[i].layout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
		}
		
		VkSubpassDescription Passes[] = {
			{
				.inputAttachmentCount = 0,
				.pInputAttachments = nullptr,
				.colorAttachmentCount = LIGHTING_PASS_INPUT_COUNT,
				.pColorAttachments = TotalReferences,
				.pDepthStencilAttachment = &TotalReferences[DEFERRED_RENDERING_ATTACHMENT_DEPTH_INDEX],
			},
			{
				.inputAttachmentCount = LIGHTING_PASS_INPUT_COUNT,
				.pInputAttachments = InputReferences,
				.colorAttachmentCount = 1,
				.pColorAttachments = &TotalReferences[DEFERRED_RENDERING_ATTACHMENT_PRESENT_INDEX],
				.pDepthStencilAttachment = nullptr,
			}
		};
		for(uint32_t i = 0; i < SIZE_OF(Passes); i++) {
			Passes[i].flags = 0;
			Passes[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			Passes[i].pResolveAttachments = nullptr;
			Passes[i].preserveAttachmentCount = 0;
			Passes[i].pPreserveAttachments = nullptr;
		}
		
		const VkSubpassDependency Dependency = {
			.srcSubpass = 0,
			.dstSubpass = 1,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
			.dependencyFlags = 0
		};
		
		const VkRenderPassCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.attachmentCount = DEFERRED_RENDERING_ATTACHMENT_COUNT,
			.pAttachments = Descriptions,
			.subpassCount = SIZE_OF(Passes),
			.pSubpasses = Passes,
			.dependencyCount = 1,
			.pDependencies = &Dependency
		};
		CHECK(vkCreateRenderPass(m_logical, &CreateInfo, nullptr, &m_deferredRenderingPass))
	}
}

void GPUFixedContext::ruin_deferredRenderingPass(void) {
	vkDestroyRenderPass(m_logical, m_deferredRenderingPass, nullptr);
}



#endif
