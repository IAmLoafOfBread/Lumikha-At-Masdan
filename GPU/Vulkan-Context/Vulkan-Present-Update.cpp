#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkPresentInfoKHR g_presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };



void GPUFixedContext::initialize_presentUpdateData(void) {
	g_presentInfo.pNext = nullptr;
	g_presentInfo.waitSemaphoreCount = 1;
	g_presentInfo.pWaitSemaphores = &m_renderFinishedSemaphore;
	g_presentInfo.swapchainCount = 1;
	g_presentInfo.pSwapchains = &m_swapchain;
	g_presentInfo.pImageIndices = &m_currentImageIndex;
	g_presentInfo.pResults = nullptr;
}

void GPUFixedContext::submit_presentUpdate(void) {
	vkQueuePresentKHR(m_deferredRenderingCommandQueue, &g_presentInfo);
}



#endif
