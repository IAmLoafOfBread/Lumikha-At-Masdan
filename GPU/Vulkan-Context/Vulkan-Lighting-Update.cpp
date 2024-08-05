#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkFence g_fences[2] = { VK_NULL_HANDLE };

static VkRenderPassBeginInfo g_lightingRenderInfo = {
	.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
	.pNext = nullptr,
	.renderPass = VK_NULL_HANDLE,
	.framebuffer = VK_NULL_HANDLE,
	.renderArea = { 0 },
	.clearValueCount = 0,
	.pClearValues = nullptr
};
static VkRenderPassBeginInfo g_postProcessingRenderInfo = {
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
		g_waitSemaphores[i] = m_shadowMappingsFinishedSemaphores[i];
	}

	g_lightingRenderInfo.renderPass = m_lightingPass;
	g_lightingRenderInfo.framebuffer = m_lightingFramebuffer;
	g_lightingRenderInfo.renderArea.extent.width = m_surfaceExtent.width;
	g_lightingRenderInfo.renderArea.extent.height = m_surfaceExtent.height;
	
	g_postProcessingRenderInfo.renderPass = m_postProcessingPass;
	g_postProcessingRenderInfo.renderArea.extent.width = m_surfaceExtent.width;
	g_postProcessingRenderInfo.renderArea.extent.height = m_surfaceExtent.height;
	
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
	
	g_postProcessingRenderInfo.framebuffer = m_postProcessingFramebuffers[m_currentImageIndex];
	
	CHECK(vkBeginCommandBuffer(m_lightingCommandSet, &G_FIXED_COMMAND_BEGIN_INFO))

	vkCmdBeginRenderPass(m_lightingCommandSet, &g_lightingRenderInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindDescriptorSets(m_lightingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_lightingLayout, 0, 1, &m_lightingDescriptorSet, 0, nullptr);
	vkCmdBindPipeline(m_lightingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_lightingPipeline);
	vkCmdPushConstants(m_lightingCommandSet, m_lightingLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(VkDeviceAddress), &m_lightAllocation.address);
	vkCmdPushConstants(m_lightingCommandSet, m_lightingLayout, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(VkDeviceAddress), sizeof(float3), &m_cameraView.position);
	vkCmdPushConstants(m_lightingCommandSet, m_lightingLayout, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(VkDeviceAddress) + sizeof(float3), sizeof(uint32_t), &m_lightCount);
	vkCmdDraw(m_lightingCommandSet, 4, 1, 0, 0);
	vkCmdEndRenderPass(m_lightingCommandSet);
	
	vkCmdBeginRenderPass(m_lightingCommandSet, &g_postProcessingRenderInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindDescriptorSets(m_lightingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_postProcessingLayout, 0, 1, &m_postProcessingDescriptorSet, 0, nullptr);
	vkCmdBindPipeline(m_lightingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_postProcessingPipeline);
	vkCmdPushConstants(m_lightingCommandSet, m_lightingLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(VkDeviceAddress), &m_reflectionSampleAllocation.address);
	vkCmdPushConstants(m_lightingCommandSet, m_lightingLayout, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(VkDeviceAddress), sizeof(VkDeviceAddress), &m_occlusionSampleAllocation.address);
	vkCmdDraw(m_lightingCommandSet, 4, 1, 0, 0);
	vkCmdEndRenderPass(m_lightingCommandSet);
	
	CHECK(vkEndCommandBuffer(m_lightingCommandSet))

	CHECK(vkResetFences(m_logical, 1, &m_lightingFinishedFence))
	CHECK(vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &g_submitInfo, m_lightingFinishedFence))

	signal_semaphore(m_lightsSemaphore);
}



#endif
