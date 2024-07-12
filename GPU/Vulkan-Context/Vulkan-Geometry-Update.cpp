#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkClearValue g_clearValues[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT + 1] = { { 0 } };
static VkRenderPassBeginInfo g_renderInfo = {  
	.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
	.pNext = nullptr,
	.renderPass = VK_NULL_HANDLE,
	.framebuffer = VK_NULL_HANDLE,
	.renderArea = { 0 },
	.clearValueCount = GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT + 1,
	.pClearValues = g_clearValues
};
static VkSubmitInfo g_submitInfo = {
	.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
	.pNext = nullptr,
	.waitSemaphoreCount = 0,
	.pWaitSemaphores = nullptr,
	.pWaitDstStageMask = nullptr,
	.commandBufferCount = 1,
	.pCommandBuffers = nullptr,
	.signalSemaphoreCount = 1,
	.pSignalSemaphores = nullptr
};



void GPUFixedContext::initialize_geometryUpdateData(void) {
	for(uint32_t i = 0; i < GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT; i++) {
		g_clearValues[i].color.float32[0] = 0;
		g_clearValues[i].color.float32[1] = 0;
		g_clearValues[i].color.float32[2] = 0;
		g_clearValues[i].color.float32[3] = 1;
	}
	g_clearValues[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT] = G_FIXED_DEPTH_CLEAR_VALUE;
	
	g_renderInfo.renderPass = m_geometryPass;
	g_renderInfo.framebuffer = m_geometryFramebuffer;
	g_renderInfo.renderArea.extent.width = m_surfaceExtent.width;
	g_renderInfo.renderArea.extent.height = m_surfaceExtent.height;
	
	g_submitInfo.pCommandBuffers = &m_geometryCommandSet;
	g_submitInfo.pSignalSemaphores = &m_geometryFinishedSemaphore;
}

void GPUFixedContext::draw_geometryUpdate(void) {
	CHECK(vkWaitForFences(m_logical, 1, &m_geometryFinishedFence, VK_TRUE, UINT64_MAX))

	CHECK(vkBeginCommandBuffer(m_geometryCommandSet, &G_FIXED_COMMAND_BEGIN_INFO))
	vkCmdBindDescriptorSets(m_geometryCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_geometryLayout, 0, 1, &m_geometryDescriptorSet, 0 , nullptr);
	vkCmdBindPipeline(m_geometryCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_geometryPipeline);
	vkCmdPushConstants(m_geometryCommandSet, m_geometryLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(View), &m_cameraView);
	vkCmdBindVertexBuffers(m_geometryCommandSet, 0, 1, &m_vertexBuffer, &m_fixedOffset);
	vkCmdBindVertexBuffers(m_geometryCommandSet, 1, 1, &m_instanceBuffer, &m_fixedOffset);
	
	vkCmdBeginRenderPass(m_geometryCommandSet, &g_renderInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdDrawIndirect(m_geometryCommandSet, m_indirectCommandBuffer, 0, m_meshCount, sizeof(VkDrawIndirectCommand));
	vkCmdEndRenderPass(m_geometryCommandSet);
	CHECK(vkEndCommandBuffer(m_geometryCommandSet))

	CHECK(vkWaitForFences(m_logical, 1, &m_swapchainFence, VK_TRUE, UINT64_MAX))
	CHECK(vkResetFences(m_logical, 1, &m_geometryFinishedFence))
	CHECK(vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &g_submitInfo, m_geometryFinishedFence))
}



#endif
