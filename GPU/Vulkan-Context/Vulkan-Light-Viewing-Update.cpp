#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkSubmitInfo g_submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };



void GPUFixedContext::initialize_lightViewingUpdateData(void) {
	g_submitInfo.pNext = nullptr;
	g_submitInfo.waitSemaphoreCount = 1;
	g_submitInfo.pWaitSemaphores = &m_imageAvailableSemaphore;
	g_submitInfo.pWaitDstStageMask = &G_FIXED_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	g_submitInfo.commandBufferCount = 1;
	g_submitInfo.pCommandBuffers = &m_lightViewingCommandSet;
	g_submitInfo.signalSemaphoreCount = 1;
	g_submitInfo.pSignalSemaphores = &m_lightViewingsFinishedSemaphore;
}

void GPUFixedContext::dispatch_lightViewingUpdate(void) {
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		wait_semaphore(m_subFrustaTransformFinishedSemaphores[i]);
	}
	
	vkBeginCommandBuffer(m_lightViewingCommandSet, &G_FIXED_COMMAND_BEGIN_INFO);
	vkCmdBindPipeline(m_lightViewingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_lightViewingPipeline);
	vkCmdPushConstants(m_lightViewingCommandSet, m_lightViewingLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(VkDeviceAddress), &m_subFrustumAllocation.address);
	vkCmdPushConstants(m_lightViewingCommandSet, m_lightViewingLayout, VK_SHADER_STAGE_COMPUTE_BIT, sizeof(VkDeviceAddress), sizeof(VkDeviceAddress), &m_lightAllocation.address);
	vkCmdPushConstants(m_lightViewingCommandSet, m_lightViewingLayout, VK_SHADER_STAGE_COMPUTE_BIT, sizeof(VkDeviceAddress) * 2, sizeof(uint32_t), &m_lightCount);
	vkCmdDispatch(m_lightViewingCommandSet, 1, (m_lightCount / MAX_WORKGROUP_SIZE) + 1, 1);
	vkEndCommandBuffer(m_lightViewingCommandSet);
	vkQueueSubmit(m_lightViewingCommandQueue, 1, &g_submitInfo, VK_NULL_HANDLE);
	
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		signal_semaphore(m_subFrustaTransformFinishedSemaphores[i]);
	}
}



#endif
