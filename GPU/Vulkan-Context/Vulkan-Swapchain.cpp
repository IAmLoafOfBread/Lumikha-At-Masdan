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
	
	auto Images = new VkImage[m_surfaceFrameCount];
	CHECK(vkGetSwapchainImagesKHR(m_logical, m_swapchain, &m_surfaceFrameCount, Images))
	
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

	const VkSubmitInfo SubmitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = nullptr,
		.pWaitDstStageMask = nullptr,
		.commandBufferCount = 1,
		.pCommandBuffers = &m_deferredRenderingCommandSet,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = nullptr
	};

	VkImageMemoryBarrier Barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = nullptr,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = VK_NULL_HANDLE,
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
	};



	for(uint32_t i = 0; i < m_surfaceFrameCount; i++) {
		CreateInfo.image = Images[i];
		CHECK(vkCreateImageView(m_logical, &CreateInfo, nullptr, &m_presentViews[i]))

		Barrier.image = Images[i];
		CHECK(vkBeginCommandBuffer(m_deferredRenderingCommandSet, &G_FIXED_COMMAND_BEGIN_INFO))
		vkCmdPipelineBarrier(m_deferredRenderingCommandSet, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &Barrier);
		vkEndCommandBuffer(m_deferredRenderingCommandSet);
		CHECK(vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &SubmitInfo, VK_NULL_HANDLE))
		CHECK(vkQueueWaitIdle(m_deferredRenderingCommandQueue))
	}
	
	delete[] Images;
}

void GPUFixedContext::ruin_swapchain(void) {
	for(uint32_t i = 0; i < m_surfaceFrameCount; i++) vkDestroyImageView(m_logical, m_presentViews[i], nullptr);
	delete[] m_presentViews;
	vkDestroySwapchainKHR(m_logical, m_swapchain, nullptr);
}



#endif
