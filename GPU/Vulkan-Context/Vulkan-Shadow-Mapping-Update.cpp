#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkPipelineStageFlags g_waitFlag = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
static VkRenderPassBeginInfo g_renderInfos[CASCADED_SHADOW_MAP_COUNT][MAX_LIGHT_COUNT] = { { { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO } } };
static VkSubmitInfo g_submitInfos[CASCADED_SHADOW_MAP_COUNT] = { { VK_STRUCTURE_TYPE_SUBMIT_INFO } };



void GPUFixedContext::initialize_shadowMappingUpdateData(void) {
	const VkExtent3D Extent = SHADOW_MAP_EXTENT;
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		for(uint32_t j = 0; j < MAX_LIGHT_COUNT; j++) {
			g_renderInfos[i][j].sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			g_renderInfos[i][j].pNext = nullptr;
			g_renderInfos[i][j].renderPass = m_shadowMappingPass;
			g_renderInfos[i][j].framebuffer = m_shadowMappingFramebuffers[i][j];
			g_renderInfos[i][j].renderArea.extent.width = Extent.width / (i + 1);
			g_renderInfos[i][j].renderArea.extent.height = Extent.height / (i + 1);
			g_renderInfos[i][j].clearValueCount = 1;
			g_renderInfos[i][j].pClearValues = &G_FIXED_DEPTH_CLEAR_VALUE;
		}

		g_submitInfos[i].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		g_submitInfos[i].pNext = nullptr;
		g_submitInfos[i].waitSemaphoreCount = 1;
		g_submitInfos[i].pWaitSemaphores = &m_lightViewingsFinishedSemaphores[i];
		g_submitInfos[i].pWaitDstStageMask = &g_waitFlag;
		g_submitInfos[i].commandBufferCount = 1;
		g_submitInfos[i].pCommandBuffers = &m_shadowMappingCommandSets[i];
		g_submitInfos[i].signalSemaphoreCount = 1;
		g_submitInfos[i].pSignalSemaphores = &m_shadowMappingsFinishedSemaphores[i];
	}
}

void GPUFixedContext::draw_shadowMappingUpdate(uint32_t in_index, uint32_t in_divisor) {
	wait_semaphore(m_instancesSemaphore);
	wait_semaphore(m_lightsSemaphore);

	CHECK(vkWaitForFences(m_logical, 1, &m_shadowMappingsFinishedFences[in_index], VK_TRUE, UINT64_MAX))

	CHECK(vkBeginCommandBuffer(m_shadowMappingCommandSets[in_index], &G_FIXED_COMMAND_BEGIN_INFO))
	vkCmdBindPipeline(m_shadowMappingCommandSets[in_index], VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadowMappingPipelines[in_index]);
	vkCmdBindVertexBuffers(m_shadowMappingCommandSets[in_index], 0, 1, &m_vertexBuffer, &m_fixedOffset);
	vkCmdBindVertexBuffers(m_shadowMappingCommandSets[in_index], 1, 1, &m_instanceBuffer, &m_fixedOffset);
	for(uint32_t i = 0; i < m_lightCount; i++) {
		if(m_lights[i].visible) {
			vkCmdBeginRenderPass(m_shadowMappingCommandSets[in_index], &g_renderInfos[in_index][i], VK_SUBPASS_CONTENTS_INLINE);
			vkCmdPushConstants(m_shadowMappingCommandSets[in_index], m_shadowMappingLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(View), &m_lights[i].view);
			vkCmdDrawIndirect(m_shadowMappingCommandSets[in_index], m_indirectCommandBuffer, 0, m_meshCount, sizeof(VkDrawIndirectCommand));
			vkCmdEndRenderPass(m_shadowMappingCommandSets[in_index]);
		}
	}
	CHECK(vkEndCommandBuffer(m_shadowMappingCommandSets[in_index]))

	CHECK(vkResetFences(m_logical, 1, &m_shadowMappingsFinishedFences[in_index]))
	CHECK(vkQueueSubmit(m_shadowMappingCommandQueues[in_index], 1, &g_submitInfos[in_index], m_shadowMappingsFinishedFences[in_index]))

	signal_semaphore(m_instancesSemaphore);
	signal_semaphore(m_lightsSemaphore);
}



#endif
