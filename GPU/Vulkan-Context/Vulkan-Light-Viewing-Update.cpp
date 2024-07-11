#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkSubmitInfo g_submitInfo = { 
	.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
	.pNext = nullptr,
	.waitSemaphoreCount = 0,
	.pWaitSemaphores = nullptr,
	.pWaitDstStageMask = nullptr,
	.commandBufferCount = 1,
	.pCommandBuffers = nullptr,
	.signalSemaphoreCount = CASCADED_SHADOW_MAP_COUNT,
	.pSignalSemaphores = nullptr
};



void GPUFixedContext::initialize_lightViewingUpdateData(void) {
	g_submitInfo.pCommandBuffers = &m_lightViewingCommandSet;
	g_submitInfo.pSignalSemaphores = m_lightViewingsFinishedSemaphores;
}

void GPUFixedContext::dispatch_lightViewingUpdate(void) {
	CHECK(vkBeginCommandBuffer(m_lightViewingCommandSet, &G_FIXED_COMMAND_BEGIN_INFO))
	vkCmdBindPipeline(m_lightViewingCommandSet, VK_PIPELINE_BIND_POINT_COMPUTE, m_lightViewingPipeline);
	vkCmdPushConstants(m_lightViewingCommandSet, m_lightViewingLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(VkDeviceAddress), &m_subFrustumAllocation.address);
	vkCmdPushConstants(m_lightViewingCommandSet, m_lightViewingLayout, VK_SHADER_STAGE_COMPUTE_BIT, sizeof(VkDeviceAddress), sizeof(VkDeviceAddress), &m_lightAllocation.address);
	vkCmdPushConstants(m_lightViewingCommandSet, m_lightViewingLayout, VK_SHADER_STAGE_COMPUTE_BIT, sizeof(VkDeviceAddress) * 2, sizeof(uint32_t), &m_lightCount);
	vkCmdDispatch(m_lightViewingCommandSet, (m_lightCount / MAX_WORKGROUP_SIZE) + 1, 1, 1);
	CHECK(vkEndCommandBuffer(m_lightViewingCommandSet))
	
	CHECK(vkWaitForFences(m_logical, 1, &m_swapchainFence, VK_TRUE, UINT64_MAX))
	CHECK(vkQueueSubmit(m_lightViewingCommandQueue, 1, &g_submitInfo, VK_NULL_HANDLE))
}



#endif
