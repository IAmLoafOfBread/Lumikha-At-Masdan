#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_swapchain(void) {
	{
		const VkSwapchainCreateInfoKHR CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.surface = m_surface,
			.minImageCount = m_surfaceFrameCount,
			.imageFormat = m_surfaceFormat,
			.imageColorSpace = m_surfaceColourSpace,
			.imageExtent = {m_surfaceExtent.width, m_surfaceExtent.height},
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &m_graphicsQueueFamilyIndex,
			.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = VK_PRESENT_MODE_FIFO_KHR,
			.clipped = VK_TRUE,
			.oldSwapchain = VK_NULL_HANDLE
		};
		CHECK(vkCreateSwapchainKHR(m_logical, &CreateInfo, nullptr, &m_swapchain));
	}
	
	m_presentImages = new GPUTextureImage[m_surfaceFrameCount];
	CHECK(vkGetSwapchainImagesKHR(m_logical, m_swapchain, &m_surfaceFrameCount, m_presentImages))

	{
		m_presentViews = new GPUTextureView[m_surfaceFrameCount];
		VkImageViewCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = VK_NULL_HANDLE,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = m_surfaceFormat,
			.components = { VK_COMPONENT_SWIZZLE_IDENTITY },
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};
		for(uint32_t i = 0; i < m_surfaceFrameCount; i++) {
			CreateInfo.image = m_presentImages[i];
			CHECK(vkCreateImageView(m_logical, &CreateInfo, nullptr, &m_presentViews[i]))
		}
	}
}

void GPUFixedContext::ruin_swapchain(void) {
	for(uint32_t i = 0; i < m_surfaceFrameCount; i++) vkDestroyImageView(m_logical, m_presentViews[i], nullptr);
	delete[] m_presentViews;
	vkDestroySwapchainKHR(m_logical, m_swapchain, nullptr);
}



#endif
