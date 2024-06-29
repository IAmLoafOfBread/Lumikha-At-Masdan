#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static const VkClearValue g_clearValue = {
	.color.float32 = { 0, 0, 0, 1}
};
static VkRenderPassBeginInfo g_renderInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
static VkPipelineStageFlags g_waitStages[CASCADED_SHADOW_MAP_COUNT] = { 0 };
static VkSubmitInfo g_submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };



void GPUFixedContext::initialize_lightingUpdateData(void) {
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
	
	vkBeginCommandBuffer(m_deferredRenderingCommandSet, &G_FIXED_COMMAND_BEGIN_INFO);
	vkCmdBindDescriptorSets(m_deferredRenderingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_lightingLayout, 0, 1, &m_lightingDescriptorSet, 0, nullptr);
	vkCmdBindPipeline(m_deferredRenderingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_lightingPipeline);
	vkCmdPushConstants(m_deferredRenderingCommandSet, m_lightingLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(uint32_t), &m_lightCount);
	
	vkCmdBeginRenderPass(m_deferredRenderingCommandSet, &g_renderInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdDraw(m_deferredRenderingCommandSet, 4, 1, 0, 0);
	vkCmdEndRenderPass(m_deferredRenderingCommandSet);
	
	vkEndCommandBuffer(m_deferredRenderingCommandSet);
	vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &g_submitInfo, VK_NULL_HANDLE);
}



#endif
