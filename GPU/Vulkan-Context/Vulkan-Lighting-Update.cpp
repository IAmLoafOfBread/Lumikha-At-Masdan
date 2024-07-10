#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkImageMemoryBarrier g_shadowBarriers[CASCADED_SHADOW_MAP_COUNT][MAX_LIGHT_COUNT] = { { { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER } } };
static VkImageMemoryBarrier g_geometryBarriers[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT] = { { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER } };

static const VkClearValue g_clearValue = {0.0f, 0.0f, 0.0f, 1.0f};
static VkRenderPassBeginInfo g_renderInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
static VkPipelineStageFlags g_waitStages[CASCADED_SHADOW_MAP_COUNT] = { 0 };
static VkSubmitInfo g_submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };



void GPUFixedContext::initialize_lightingUpdateData(void) {
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		for(uint32_t j = 0; j < MAX_LIGHT_COUNT; j++) {
			g_shadowBarriers[i][j].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			g_shadowBarriers[i][j].pNext = nullptr;
			g_shadowBarriers[i][j].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			g_shadowBarriers[i][j].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			g_shadowBarriers[i][j].oldLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
			g_shadowBarriers[i][j].newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			g_shadowBarriers[i][j].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			g_shadowBarriers[i][j].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			g_shadowBarriers[i][j].image = m_shadowTextures[i][j].image;
			g_shadowBarriers[i][j].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			g_shadowBarriers[i][j].subresourceRange.baseMipLevel = 0;
			g_shadowBarriers[i][j].subresourceRange.levelCount = 1;
			g_shadowBarriers[i][j].subresourceRange.baseArrayLayer = 0;
			g_shadowBarriers[i][j].subresourceRange.layerCount = 1;
		}
	}

	for (uint32_t i = 0; i < GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT; i++) {
		g_geometryBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		g_geometryBarriers[i].pNext = nullptr;
		g_geometryBarriers[i].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		g_geometryBarriers[i].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		g_geometryBarriers[i].oldLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
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

	g_renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	g_renderInfo.pNext = nullptr;
	g_renderInfo.renderPass = m_lightingPass;
	g_renderInfo.framebuffer = VK_NULL_HANDLE;
	g_renderInfo.renderArea.offset.x = 0;
	g_renderInfo.renderArea.offset.y = 0;
	g_renderInfo.renderArea.extent.width = m_surfaceExtent.width;
	g_renderInfo.renderArea.extent.height = m_surfaceExtent.height;
	g_renderInfo.clearValueCount = 1;
	g_renderInfo.pClearValues = &g_clearValue;
	
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		g_waitStages[i] = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	
	g_submitInfo.pNext = nullptr;
	g_submitInfo.waitSemaphoreCount = CASCADED_SHADOW_MAP_COUNT;
	g_submitInfo.pWaitSemaphores = m_shadowMappingsFinishedSemaphores;
	g_submitInfo.pWaitDstStageMask = g_waitStages;
	g_submitInfo.commandBufferCount = 1;
	g_submitInfo.pCommandBuffers = &m_deferredRenderingCommandSet;
	g_submitInfo.signalSemaphoreCount = 1;
	g_submitInfo.pSignalSemaphores = &m_renderFinishedSemaphore;
}

void GPUFixedContext::draw_lightingUpdate(void) {
	g_renderInfo.framebuffer = m_lightingFramebuffers[m_currentImageIndex];
	
	CHECK(vkBeginCommandBuffer(m_deferredRenderingCommandSet, &G_FIXED_COMMAND_BEGIN_INFO))
	vkCmdBindDescriptorSets(m_deferredRenderingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_lightingLayout, 0, 1, &m_lightingDescriptorSet, 0, nullptr);
	vkCmdBindPipeline(m_deferredRenderingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_lightingPipeline);
	vkCmdPushConstants(m_deferredRenderingCommandSet, m_lightingLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(uint32_t), &m_lightCount);
	
	vkCmdBeginRenderPass(m_deferredRenderingCommandSet, &g_renderInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdDraw(m_deferredRenderingCommandSet, 4, 1, 0, 0);
	vkCmdEndRenderPass(m_deferredRenderingCommandSet);
	
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		vkCmdPipelineBarrier(m_deferredRenderingCommandSet, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, nullptr, 0, nullptr, m_lightCount, g_shadowBarriers[i]);
	}
	vkCmdPipelineBarrier(m_deferredRenderingCommandSet, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT, g_geometryBarriers);
	
	CHECK(vkEndCommandBuffer(m_deferredRenderingCommandSet))
	CHECK(vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &g_submitInfo, VK_NULL_HANDLE))
}



#endif
