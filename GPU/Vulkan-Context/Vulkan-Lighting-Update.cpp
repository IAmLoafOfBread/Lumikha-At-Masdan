#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkFence g_fences[2] = { VK_NULL_HANDLE };

static VkImageMemoryBarrier g_shadowBarriers[CASCADED_SHADOW_MAP_COUNT][MAX_LIGHT_COUNT] = { { { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER } } };
static VkImageMemoryBarrier g_geometryBarriers[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT] = { { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER } };

static VkRenderPassBeginInfo g_renderInfo = {
	.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
	.pNext = nullptr,
	.renderPass = VK_NULL_HANDLE,
	.framebuffer = VK_NULL_HANDLE,
	.renderArea = { 0 },
	.clearValueCount = 0,
	.pClearValues = nullptr
};
static VkPipelineStageFlags g_waitStages[CASCADED_SHADOW_MAP_COUNT + 1] = { 0 };
static VkSemaphore g_waitSemaphores[CASCADED_SHADOW_MAP_COUNT + 1] = { VK_NULL_HANDLE };
static VkSubmitInfo g_submitInfo = {
	.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
	.pNext = nullptr,
	.waitSemaphoreCount = CASCADED_SHADOW_MAP_COUNT + 1,
	.pWaitSemaphores = g_waitSemaphores,
	.pWaitDstStageMask = g_waitStages,
	.commandBufferCount = 1,
	.pCommandBuffers = nullptr,
	.signalSemaphoreCount = 1,
	.pSignalSemaphores = nullptr
};



void GPUFixedContext::initialize_lightingUpdateData(void) {
	g_fences[0] = m_swapchainFence;
	g_fences[1] = m_lightingFinishedFence;

	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		for(uint32_t j = 0; j < MAX_LIGHT_COUNT; j++) {
			g_shadowBarriers[i][j].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			g_shadowBarriers[i][j].pNext = nullptr;
			g_shadowBarriers[i][j].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			g_shadowBarriers[i][j].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			g_shadowBarriers[i][j].oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			g_shadowBarriers[i][j].newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			g_shadowBarriers[i][j].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			g_shadowBarriers[i][j].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			g_shadowBarriers[i][j].image = m_shadowTextures[i][j].image;
			g_shadowBarriers[i][j].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			g_shadowBarriers[i][j].subresourceRange.baseMipLevel = 0;
			g_shadowBarriers[i][j].subresourceRange.levelCount = 1;
			g_shadowBarriers[i][j].subresourceRange.baseArrayLayer = 0;
			g_shadowBarriers[i][j].subresourceRange.layerCount = 1;
		}
		g_waitSemaphores[i] = m_shadowMappingsFinishedSemaphores[i];
	}

	for (uint32_t i = 0; i < GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT; i++) {
		g_geometryBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		g_geometryBarriers[i].pNext = nullptr;
		g_geometryBarriers[i].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		g_geometryBarriers[i].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		g_geometryBarriers[i].oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		g_geometryBarriers[i].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		g_geometryBarriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		g_geometryBarriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		g_geometryBarriers[i].image = m_geometryTextures[i].image;
		g_geometryBarriers[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		g_geometryBarriers[i].subresourceRange.baseMipLevel = 0;
		g_geometryBarriers[i].subresourceRange.levelCount = 1;
		g_geometryBarriers[i].subresourceRange.baseArrayLayer = 0;
		g_geometryBarriers[i].subresourceRange.layerCount = 1;
	}

	g_renderInfo.renderPass = m_lightingPass;
	g_renderInfo.renderArea.extent.width = m_surfaceExtent.width;
	g_renderInfo.renderArea.extent.height = m_surfaceExtent.height;
	
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT + 1; i++) {
		g_waitStages[i] = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	g_waitSemaphores[CASCADED_SHADOW_MAP_COUNT] = m_geometryFinishedSemaphore;
	
	g_submitInfo.pCommandBuffers = &m_lightingCommandSet;
	g_submitInfo.pSignalSemaphores = &m_lightingFinishedSemaphore;
}

void GPUFixedContext::draw_lightingUpdate(void) {
	wait_semaphore(m_lightsSemaphore);

	CHECK(vkWaitForFences(m_logical, LENGTH_OF(g_fences), g_fences, VK_TRUE, UINT64_MAX))
	g_renderInfo.framebuffer = m_lightingFramebuffers[m_currentImageIndex];
	
	CHECK(vkBeginCommandBuffer(m_lightingCommandSet, &G_FIXED_COMMAND_BEGIN_INFO))
	vkCmdBindDescriptorSets(m_lightingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_lightingLayout, 0, 1, &m_lightingDescriptorSet, 0, nullptr);
	vkCmdBindPipeline(m_lightingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_lightingPipeline);
	vkCmdPushConstants(m_lightingCommandSet, m_lightingLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(VkDeviceAddress), &m_lightAllocation.address);
	vkCmdPushConstants(m_lightingCommandSet, m_lightingLayout, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(VkDeviceAddress), sizeof(float3), &m_cameraView.position);
	vkCmdPushConstants(m_lightingCommandSet, m_lightingLayout, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(VkDeviceAddress) + sizeof(float3), sizeof(uint32_t), &m_lightCount);
	
	vkCmdBeginRenderPass(m_lightingCommandSet, &g_renderInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdDraw(m_lightingCommandSet, 4, 1, 0, 0);
	vkCmdEndRenderPass(m_lightingCommandSet);
	
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		vkCmdPipelineBarrier(m_lightingCommandSet, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, nullptr, 0, nullptr, MAX_LIGHT_COUNT, g_shadowBarriers[i]);
	}
	vkCmdPipelineBarrier(m_lightingCommandSet, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT, g_geometryBarriers);
	CHECK(vkEndCommandBuffer(m_lightingCommandSet))

	CHECK(vkResetFences(m_logical, 1, &m_lightingFinishedFence))
	CHECK(vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &g_submitInfo, m_lightingFinishedFence))

	signal_semaphore(m_lightsSemaphore);
}



#endif
