#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkFence g_fence = VK_NULL_HANDLE;
static VkSemaphoreSignalInfo g_signalInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO };



void GPUFixedContext::initialize_imageAcquiring(void) {
	{
		const struct VkFenceCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0
		};
		CHECK(vkCreateFence(m_logical, &CreateInfo, nullptr, &g_fence))
	}

	g_signalInfo.pNext = nullptr;
	g_signalInfo.semaphore = m_imageAvailableSemaphore;
	g_signalInfo.value = 2;
}

void GPUFixedContext::acquire_nextImage(void) {
	m_imageAvailableStatus = 2;

	vkResetFences(m_logical, 1, &g_fence);
	vkAcquireNextImageKHR(m_logical, m_swapchain, UINT64_MAX, VK_NULL_HANDLE, g_fence, &m_currentImageIndex);
	vkWaitForFences(m_logical, 1, &g_fence, VK_TRUE, UINT64_MAX);
	vkSignalSemaphore(m_logical, &g_signalInfo);
}



#endif
