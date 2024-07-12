#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkFence g_fences[2] = { VK_NULL_HANDLE };

static VkImageMemoryBarrier g_barrier = {
	.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
	.pNext = nullptr,
	.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
	.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	.image = VK_NULL_HANDLE,
	.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT }
};

static VkPipelineStageFlags g_waitStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
static VkSubmitInfo g_submitInfo = {
	.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
	.pNext = nullptr,
	.waitSemaphoreCount = 1,
	.pWaitSemaphores = nullptr,
	.pWaitDstStageMask = &g_waitStage,
	.commandBufferCount = 1,
	.pCommandBuffers = nullptr,
	.signalSemaphoreCount = 0,
	.pSignalSemaphores = nullptr
};

static VkPresentInfoKHR g_presentInfo = {
	.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
	.pNext = nullptr,
	.waitSemaphoreCount = 1,
	.pWaitSemaphores = nullptr,
	.swapchainCount = 1,
	.pSwapchains = nullptr,
	.pImageIndices = nullptr,
	.pResults = nullptr
};



void GPUFixedContext::initialize_presentUpdateData(void) {
	g_fences[0] = m_swapchainFence;
	g_fences[1] = m_presentFinishedFence;

	g_barrier.subresourceRange.levelCount = 1;
	g_barrier.subresourceRange.layerCount = 1;

	g_submitInfo.pWaitSemaphores = &m_lightingFinishedSemaphores[0];
	g_submitInfo.pCommandBuffers = &m_presentCommandSet;

	g_presentInfo.pWaitSemaphores = &m_lightingFinishedSemaphores[1];
	g_presentInfo.pSwapchains = &m_swapchain;
	g_presentInfo.pImageIndices = &m_currentImageIndex;
}

void GPUFixedContext::submit_presentUpdate(void) {
	CHECK(vkWaitForFences(m_logical, LENGTH_OF(g_fences), g_fences, VK_TRUE, UINT64_MAX))

	g_barrier.image = m_presentImages[m_currentImageIndex];

	CHECK(vkBeginCommandBuffer(m_presentCommandSet, &G_FIXED_COMMAND_BEGIN_INFO))
	vkCmdPipelineBarrier(m_presentCommandSet, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &g_barrier);
	vkEndCommandBuffer(m_presentCommandSet);

	CHECK(vkResetFences(m_logical, 1, &m_presentFinishedFence))
	CHECK(vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &g_submitInfo, m_presentFinishedFence))

	CHECK(vkQueuePresentKHR(m_deferredRenderingCommandQueue, &g_presentInfo))
}



#endif
