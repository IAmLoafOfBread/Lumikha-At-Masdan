#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkRenderPassBeginInfo g_renderInfos[CASCADED_SHADOW_MAP_COUNT] = { { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO } };
static uint64_t g_signalValue = 1;
static VkTimelineSemaphoreSubmitInfo g_timelineInfo = { VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO };
static VkSubmitInfo g_submitInfos[CASCADED_SHADOW_MAP_COUNT] = { { VK_STRUCTURE_TYPE_SUBMIT_INFO } };


void GPUFixedContext::initialize_shadowMappingUpdateData(void) {
	g_timelineInfo.pNext = nullptr;
	g_timelineInfo.waitSemaphoreValueCount = 1;
	g_timelineInfo.pWaitSemaphoreValues = &m_lightViewingsFinishedStatus;
	g_timelineInfo.signalSemaphoreValueCount = 0;
	g_timelineInfo.pSignalSemaphoreValues = nullptr;

	const VkExtent3D Extent = SHADOW_MAP_EXTENT;
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		g_renderInfos[i].sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		g_renderInfos[i].pNext = nullptr;
		g_renderInfos[i].renderPass = m_shadowMappingPass;
		g_renderInfos[i].framebuffer = VK_NULL_HANDLE;
		g_renderInfos[i].renderArea.offset.x = 0;
		g_renderInfos[i].renderArea.offset.y = 0;
		g_renderInfos[i].renderArea.extent.width = Extent.width / (i + 1);
		g_renderInfos[i].renderArea.extent.height = Extent.height / (i + 1);
		g_renderInfos[i].clearValueCount = 1;
		g_renderInfos[i].pClearValues = &G_FIXED_DEPTH_CLEAR_VALUE;
	

		g_submitInfos[i].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		g_submitInfos[i].pNext = &g_timelineInfo;
		g_submitInfos[i].waitSemaphoreCount = 1;
		g_submitInfos[i].pWaitSemaphores = &m_lightViewingsFinishedSemaphore;
		g_submitInfos[i].pWaitDstStageMask = &G_FIXED_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		g_submitInfos[i].commandBufferCount = 1;
		g_submitInfos[i].pCommandBuffers = &m_shadowMappingCommandSets[i];
		g_submitInfos[i].signalSemaphoreCount = 1;
		g_submitInfos[i].pSignalSemaphores = &m_shadowMappingsFinishedSemaphores[i];
	}
}

void GPUFixedContext::draw_shadowMappingUpdate(uint32_t in_index, uint32_t in_divisor) {
	m_lightViewingsFinishedStatus--;

	for(uint32_t i = 0; i < m_lightCount; i++) {
		if(m_lights[i].visible) {
			g_renderInfos[in_index].framebuffer = m_shadowMappingFramebuffers[in_index][i];
			
			CHECK(vkBeginCommandBuffer(m_shadowMappingCommandSets[in_index], &G_FIXED_COMMAND_BEGIN_INFO))
			vkCmdBindPipeline(m_shadowMappingCommandSets[in_index], VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadowMappingPipelines[in_index]);
			vkCmdBindVertexBuffers(m_shadowMappingCommandSets[in_index], 0, 1, &m_vertexBuffer, &m_fixedOffset);
			vkCmdBindVertexBuffers(m_shadowMappingCommandSets[in_index], 1, 1, &m_instanceBuffer, &m_fixedOffset);
			
			vkCmdPushConstants(m_shadowMappingCommandSets[in_index], m_shadowMappingLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(View), &m_lights[i].view);
			vkCmdBeginRenderPass(m_shadowMappingCommandSets[in_index], &g_renderInfos[i], VK_SUBPASS_CONTENTS_INLINE);
			vkCmdDrawIndirect(m_shadowMappingCommandSets[in_index], m_indirectCommandBuffer, 0, m_meshCount, sizeof(VkDrawIndirectCommand));
			vkCmdEndRenderPass(m_shadowMappingCommandSets[in_index]);
			
			CHECK(vkEndCommandBuffer(m_shadowMappingCommandSets[in_index]))
			CHECK(vkQueueSubmit(m_shadowMappingCommandQueues[in_index], 1, &g_submitInfos[i], VK_NULL_HANDLE))
		}
	}
}



#endif
