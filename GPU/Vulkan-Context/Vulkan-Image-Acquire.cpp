#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::acquire_nextImageUpdate(void) {
	CHECK(vkResetFences(m_logical, 1, &m_swapchainFence))
	CHECK(vkAcquireNextImageKHR(m_logical, m_swapchain, UINT64_MAX, VK_NULL_HANDLE, m_swapchainFence, &m_currentImageIndex))
}



#endif
