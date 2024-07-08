#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkClearValue g_clearValues[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT + 1] = { 0 };
static VkRenderPassBeginInfo g_renderInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
static VkTimelineSemaphoreSubmitInfo g_timelineInfo = { VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO };
static VkSubmitInfo g_submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };



void GPUFixedContext::initialize_geometryUpdateData(void) {
	for(uint32_t i = 0; i < GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT; i++) {
		g_clearValues[i].color.float32[0] = 0;
		g_clearValues[i].color.float32[1] = 0;
		g_clearValues[i].color.float32[2] = 0;
		g_clearValues[i].color.float32[3] = 1;
	}
	g_clearValues[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT] = G_FIXED_DEPTH_CLEAR_VALUE;
	
	g_renderInfo.pNext = nullptr;
	g_renderInfo.renderPass = m_geometryPass;
	g_renderInfo.framebuffer = m_geometryFramebuffer;
	g_renderInfo.renderArea.offset.x = 0;
	g_renderInfo.renderArea.offset.y = 0;
	g_renderInfo.renderArea.extent.width = m_surfaceExtent.width;
	g_renderInfo.renderArea.extent.height = m_surfaceExtent.height;
	g_renderInfo.clearValueCount = GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT + 1;
	g_renderInfo.pClearValues = g_clearValues;
	
	g_timelineInfo.pNext = nullptr;
	g_timelineInfo.waitSemaphoreValueCount = 1;
	g_timelineInfo.pWaitSemaphoreValues = &m_imageAvailableStatus;
	g_timelineInfo.signalSemaphoreValueCount = 0;
	g_timelineInfo.pSignalSemaphoreValues = nullptr;
	
	g_submitInfo.pNext = &g_timelineInfo;
	g_submitInfo.waitSemaphoreCount = 1;
	g_submitInfo.pWaitSemaphores = &m_imageAvailableSemaphore;
	g_submitInfo.pWaitDstStageMask = &G_FIXED_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	g_submitInfo.commandBufferCount = 1;
	g_submitInfo.pCommandBuffers = &m_deferredRenderingCommandSet;
	g_submitInfo.signalSemaphoreCount = 0;
	g_submitInfo.pSignalSemaphores = nullptr;
}

void GPUFixedContext::draw_geometryUpdate(void) {
	m_imageAvailableStatus--;

	vkBeginCommandBuffer(m_deferredRenderingCommandSet, &G_FIXED_COMMAND_BEGIN_INFO);
	vkCmdBindDescriptorSets(m_deferredRenderingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_geometryLayout, 0, 1, &m_geometryDescriptorSet, 0 , nullptr);
	vkCmdBindPipeline(m_deferredRenderingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_geometryPipeline);
	vkCmdPushConstants(m_deferredRenderingCommandSet, m_geometryLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(View), &m_cameraView);
	vkCmdBindVertexBuffers(m_deferredRenderingCommandSet, 0, 1, &m_vertexBuffer, &m_fixedOffset);
	vkCmdBindVertexBuffers(m_deferredRenderingCommandSet, 1, 1, &m_instanceBuffer, &m_fixedOffset);
	
	vkCmdBeginRenderPass(m_deferredRenderingCommandSet, &g_renderInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdDrawIndirect(m_deferredRenderingCommandSet, m_indirectCommandBuffer, 0, m_meshCount, sizeof(VkDrawIndirectCommand));
	vkCmdEndRenderPass(m_deferredRenderingCommandSet);
	
	vkEndCommandBuffer(m_deferredRenderingCommandSet);
	vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &g_submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_deferredRenderingCommandQueue);
}



#endif
