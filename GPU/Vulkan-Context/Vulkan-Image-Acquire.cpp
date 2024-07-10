#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkSemaphoreSignalInfo g_signalInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO };



void GPUFixedContext::initialize_imageAcquiringUpdateData(void) {
	g_signalInfo.pNext = nullptr;
	g_signalInfo.semaphore = m_imageAvailableSemaphore;
	g_signalInfo.value = 2;
}

void GPUFixedContext::acquire_nextImageUpdate(void) {
	m_imageAvailableStatus = 2;

	CHECK(vkResetFences(m_logical, 1, &m_swapchainFence))
	CHECK(vkAcquireNextImageKHR(m_logical, m_swapchain, UINT64_MAX, VK_NULL_HANDLE, m_swapchainFence, &m_currentImageIndex))
	CHECK(vkWaitForFences(m_logical, 1, &m_swapchainFence, VK_TRUE, UINT64_MAX))
	CHECK(vkSignalSemaphore(m_logical, &g_signalInfo))
}



#endif
