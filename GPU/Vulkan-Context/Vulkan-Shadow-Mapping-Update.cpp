#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkRenderPassBeginInfo g_renderInfos[CASCADED_SHADOW_MAP_COUNT] = { { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO } };
static VkSubmitInfo g_submitInfos[CASCADED_SHADOW_MAP_COUNT] = { { VK_STRUCTURE_TYPE_SUBMIT_INFO } };


void GPUFixedContext::initialize_shadowMappingUpdateData(uint32_t in_index, uint32_t in_divisor) {
	const VkExtent3D Extent = SHADOW_MAP_EXTENT;
	g_renderInfos[in_index].sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	g_renderInfos[in_index].pNext = nullptr;
	g_renderInfos[in_index].renderPass = m_shadowMappingPass;
	g_renderInfos[in_index].framebuffer = VK_NULL_HANDLE;
	g_renderInfos[in_index].renderArea.offset.x = 0;
	g_renderInfos[in_index].renderArea.offset.y = 0;
	g_renderInfos[in_index].renderArea.extent.width = Extent.width / in_divisor;
	g_renderInfos[in_index].renderArea.extent.height = Extent.height / in_divisor;
	g_renderInfos[in_index].clearValueCount = 1;
	g_renderInfos[in_index].pClearValues = &G_FIXED_DEPTH_CLEAR_VALUE;
	
	g_submitInfos[in_index].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	g_submitInfos[in_index].pNext = nullptr;
	g_submitInfos[in_index].waitSemaphoreCount = 1;
	g_submitInfos[in_index].pWaitSemaphores = &m_lightViewingsFinishedSemaphore;
	g_submitInfos[in_index].pWaitDstStageMask = &G_FIXED_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	g_submitInfos[in_index].commandBufferCount = 1;
	g_submitInfos[in_index].pCommandBuffers = &m_shadowMappingCommandSets[in_index];
	g_submitInfos[in_index].signalSemaphoreCount = 1;
	g_submitInfos[in_index].pSignalSemaphores = &m_shadowMappingsFinishedSemaphores[in_index];
}

void GPUFixedContext::draw_shadowMappingUpdate(uint32_t in_index, uint32_t in_divisor) {
	wait_semaphore(m_subFrustaTransformFinishedSemaphores[in_index]);
	calculate_subFrustum(&static_cast<float3*>(m_subFrustumAllocation.data)[CORNER_COUNT * in_index], &m_cameraView, in_divisor);
	signal_semaphore(m_subFrustaTransformFinishedSemaphores[in_index]);
	
	for(uint32_t i = 0; i < m_lightCount; i++) {
		if(m_lights[i].visible) {
			g_renderInfos[in_index].framebuffer = m_shadowMappingFramebuffers[in_index][i];
			
			vkBeginCommandBuffer(m_shadowMappingCommandSets[in_index], &G_FIXED_COMMAND_BEGIN_INFO);
			vkCmdBindPipeline(m_shadowMappingCommandSets[in_index], VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadowMappingPipelines[in_index]);
			vkCmdBindVertexBuffers(m_shadowMappingCommandSets[in_index], 0, 1, &m_vertexBuffer, &m_fixedOffset);
			vkCmdBindVertexBuffers(m_shadowMappingCommandSets[in_index], 1, 1, &m_instanceBuffer, &m_fixedOffset);
			
			vkCmdPushConstants(m_shadowMappingCommandSets[in_index], m_shadowMappingLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(View), &m_lights[i].view);
			vkCmdBeginRenderPass(m_shadowMappingCommandSets[in_index], &g_renderInfos[i], VK_SUBPASS_CONTENTS_INLINE);
			vkCmdDrawIndirect(m_shadowMappingCommandSets[in_index], m_indirectCommandBuffer, 0, m_meshCount, sizeof(VkDrawIndirectCommand));
			vkCmdEndRenderPass(m_shadowMappingCommandSets[in_index]);
			
			vkEndCommandBuffer(m_shadowMappingCommandSets[in_index]);
			vkQueueSubmit(m_shadowMappingCommandQueues[in_index], 1, &g_submitInfos[i], VK_NULL_HANDLE);
		}
	}
}



#endif
