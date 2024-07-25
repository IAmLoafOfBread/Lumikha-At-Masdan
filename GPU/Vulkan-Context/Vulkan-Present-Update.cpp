#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



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
	g_presentInfo.pWaitSemaphores = &m_lightingFinishedSemaphore;
	g_presentInfo.pSwapchains = &m_swapchain;
	g_presentInfo.pImageIndices = &m_currentImageIndex;
}

void GPUFixedContext::submit_presentUpdate(void) {
	CHECK(vkWaitForFences(m_logical, 1, &m_swapchainFence, VK_TRUE, UINT64_MAX))
	CHECK(vkQueuePresentKHR(m_deferredRenderingCommandQueue, &g_presentInfo))
}



#endif
