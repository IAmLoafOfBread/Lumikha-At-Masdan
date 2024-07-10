#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkImageMemoryBarrier g_barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };

static VkPresentInfoKHR g_presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };

static VkSubmitInfo g_submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };



void GPUFixedContext::initialize_presentUpdateData(void) {
	g_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	g_barrier.pNext = nullptr;
	g_barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	g_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	g_barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	g_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	g_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	g_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	g_barrier.image = VK_NULL_HANDLE;
	g_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	g_barrier.subresourceRange.baseMipLevel = 0;
	g_barrier.subresourceRange.levelCount = 1;
	g_barrier.subresourceRange.baseArrayLayer = 0;
	g_barrier.subresourceRange.layerCount = 1;

	g_presentInfo.pNext = nullptr;
	g_presentInfo.waitSemaphoreCount = 1;
	g_presentInfo.pWaitSemaphores = &m_renderFinishedSemaphore;
	g_presentInfo.swapchainCount = 1;
	g_presentInfo.pSwapchains = &m_swapchain;
	g_presentInfo.pImageIndices = &m_currentImageIndex;
	g_presentInfo.pResults = nullptr;

	g_submitInfo.pNext = nullptr;
	g_submitInfo.waitSemaphoreCount = 0;
	g_submitInfo.pWaitSemaphores = nullptr;
	g_submitInfo.pWaitDstStageMask = nullptr;
	g_submitInfo.commandBufferCount = 1;
	g_submitInfo.pCommandBuffers = &m_deferredRenderingCommandSet;
	g_submitInfo.signalSemaphoreCount = 0;
	g_submitInfo.pSignalSemaphores = nullptr;
}

void GPUFixedContext::submit_presentUpdate(void) {
	g_barrier.image = m_presentImages[m_currentImageIndex];
	vkQueuePresentKHR(m_deferredRenderingCommandQueue, &g_presentInfo);
	CHECK(vkQueueWaitIdle(m_deferredRenderingCommandQueue))

	CHECK(vkBeginCommandBuffer(m_deferredRenderingCommandSet, &G_FIXED_COMMAND_BEGIN_INFO))
	vkCmdPipelineBarrier(m_deferredRenderingCommandSet, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &g_barrier);
	vkEndCommandBuffer(m_deferredRenderingCommandSet);
	CHECK(vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &g_submitInfo, VK_NULL_HANDLE))
	CHECK(vkQueueWaitIdle(m_deferredRenderingCommandQueue))
}



#endif
