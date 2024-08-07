#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"




void GPUFixedContext::build_stageAllocation(GPUStageAllocation* in_stage, GPUSize in_size) {
	in_stage->size = in_size;
	{
		const VkBufferCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = in_size,
			.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &m_graphicsQueueFamilyIndex
		};
		CHECK(vkCreateBuffer(m_logical, &CreateInfo, nullptr, &in_stage->buffer))
	}
	VkMemoryRequirements Requirements = {};
	vkGetBufferMemoryRequirements(m_logical, in_stage->buffer, &Requirements);
	const VkMemoryAllocateInfo AllocInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = nullptr,
		.allocationSize = Requirements.size,
		.memoryTypeIndex = m_sharedMemoryIndex
	};
	CHECK(vkAllocateMemory(m_logical, &AllocInfo, nullptr, &in_stage->memory))
	CHECK(vkBindBufferMemory(m_logical, in_stage->buffer, in_stage->memory, 0))
	CHECK(vkMapMemory(m_logical, in_stage->memory, 0, Requirements.size, 0, &in_stage->data))
}



void GPUFixedContext::build_sharedAllocation(GPUSharedAllocation* in_shared, GPUSize in_size, GPUBufferUsageFlags in_flags, bool in_computeIndexInclude) {
	{
		{
			const uint32_t Indices[] = {
				m_graphicsQueueFamilyIndex,
				m_computeQueueFamilyIndex
			};
			const VkBufferCreateInfo CreateInfo = {
				.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.size = in_size,
				.usage = in_flags,
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = in_computeIndexInclude ? m_queueFamilyCount : 1,
				.pQueueFamilyIndices = Indices
			};
			CHECK(vkCreateBuffer(m_logical, &CreateInfo, nullptr, &in_shared->buffer))
		}
	}
	VkMemoryRequirements Requirements = {};
	vkGetBufferMemoryRequirements(m_logical, in_shared->buffer, &Requirements);
	const VkMemoryAllocateFlagsInfo Flags = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
		.pNext = nullptr,
		.flags = static_cast<VkMemoryAllocateFlags>(in_flags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT ? VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT : 0),
		.deviceMask = 0
	};
	const VkMemoryAllocateInfo AllocInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = &Flags,
		.allocationSize = Requirements.size,
		.memoryTypeIndex = m_sharedMemoryIndex
	};
	CHECK(vkAllocateMemory(m_logical, &AllocInfo, nullptr, &in_shared->memory))
	CHECK(vkBindBufferMemory(m_logical, in_shared->buffer, in_shared->memory, 0))
	CHECK(vkMapMemory(m_logical, in_shared->memory, 0, Requirements.size, 0, &in_shared->data))
	if(in_flags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
		const VkBufferDeviceAddressInfo Info = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
			.pNext = nullptr,
			.buffer = in_shared->buffer
		};
		in_shared->address = vkGetBufferDeviceAddress(m_logical, &Info);
	}
}

void GPUFixedContext::ruin_sharedAllocation(GPUSharedAllocation* in_shared) {
	vkUnmapMemory(m_logical, in_shared->memory);
	vkFreeMemory(m_logical, in_shared->memory, nullptr);
	vkDestroyBuffer(m_logical, in_shared->buffer, nullptr);
}



void GPUFixedContext::build_localAllocation(GPULocalAllocation* in_local, GPUStageAllocation* in_stage, GPUBufferUsageFlags in_flags) {
	vkUnmapMemory(m_logical, in_stage->memory);
	
	{
		const VkBufferCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = in_stage->size,
			.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | in_flags,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &m_graphicsQueueFamilyIndex
		};
		CHECK(vkCreateBuffer(m_logical, &CreateInfo, nullptr, &in_local->buffer))
	}
	
	VkMemoryRequirements Requirements = {};
	
	{
		vkGetBufferMemoryRequirements(m_logical, in_local->buffer, &Requirements);
		const VkMemoryAllocateFlagsInfo Flags = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
			.pNext = nullptr,
			.flags = static_cast<VkMemoryAllocateFlags>(in_flags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT ? VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT : 0),
			.deviceMask = 0
		};
		const VkMemoryAllocateInfo AllocInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = &Flags,
			.allocationSize = Requirements.size,
			.memoryTypeIndex = m_localMemoryIndex
		};
		CHECK(vkAllocateMemory(m_logical, &AllocInfo, nullptr, &in_local->memory))
		CHECK(vkBindBufferMemory(m_logical, in_local->buffer, in_local->memory, 0))
		if(in_flags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
			const VkBufferDeviceAddressInfo Info = {
				.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
				.pNext = nullptr,
				.buffer = in_local->buffer
			};
			in_local->address = vkGetBufferDeviceAddress(m_logical, &Info);
		}
	}
	
	const VkBufferCopy Region = {
		.srcOffset = 0,
		.dstOffset = 0,
		.size = Requirements.size
	};

	const VkSubmitInfo SubmitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = nullptr,
		.pWaitDstStageMask = nullptr,
		.commandBufferCount = 1,
		.pCommandBuffers = &m_presentCommandSet,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = nullptr
	};
	CHECK(vkBeginCommandBuffer(m_presentCommandSet, &G_FIXED_COMMAND_BEGIN_INFO))
	vkCmdCopyBuffer(m_presentCommandSet, in_stage->buffer, in_local->buffer, 1, &Region);
	vkEndCommandBuffer(m_presentCommandSet);
	CHECK(vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &SubmitInfo, VK_NULL_HANDLE))
	CHECK(vkQueueWaitIdle(m_deferredRenderingCommandQueue))
	
	vkFreeMemory(m_logical, in_stage->memory, nullptr);
	vkDestroyBuffer(m_logical, in_stage->buffer, nullptr);
}

void GPUFixedContext::ruin_localAllocation(GPULocalAllocation* in_local) {
	vkFreeMemory(m_logical, in_local->memory, nullptr);
	vkDestroyBuffer(m_logical, in_local->buffer, nullptr);
}



void GPUFixedContext::build_localTexture(GPULocalTexture* in_texture, GPUStageAllocation* in_stage, GPUFormat in_format, GPUExtent3D in_extent, GPUImageUsageFlags in_flags, GPUImageLayout in_layout) {
	{
		const VkImageCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = in_extent.depth > 1 ? VK_IMAGE_TYPE_3D : VK_IMAGE_TYPE_2D,
			.format = in_format,
			.extent = in_extent,
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = in_flags | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &m_graphicsQueueFamilyIndex,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};
		CHECK(vkCreateImage(m_logical, &CreateInfo, nullptr, &in_texture->image))
	}
	{
		VkMemoryRequirements Requirements = {};
		vkGetImageMemoryRequirements(m_logical, in_texture->image, &Requirements);
		const VkMemoryAllocateInfo AllocInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = Requirements.size,
			.memoryTypeIndex = m_localMemoryIndex
		};
		CHECK(vkAllocateMemory(m_logical, &AllocInfo, nullptr, &in_texture->memory))
		CHECK(vkBindImageMemory(m_logical, in_texture->image, in_texture->memory, 0))
	}
	{
		const VkImageViewCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = in_texture->image,
			.viewType = in_extent.depth > 1 ? VK_IMAGE_VIEW_TYPE_3D : VK_IMAGE_VIEW_TYPE_2D,
			.format = in_format,
			.components = { VK_COMPONENT_SWIZZLE_IDENTITY },
			.subresourceRange = {
				.aspectMask = static_cast<VkImageAspectFlags>(in_flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT),
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};
		CHECK(vkCreateImageView(m_logical, &CreateInfo, nullptr, &in_texture->view))
	}
	
	VkAccessFlags AccessFlags = 0;
	if(in_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		AccessFlags = VK_ACCESS_SHADER_READ_BIT;
	}
	if(in_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		AccessFlags = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}
	if(in_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		AccessFlags = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}

	VkPipelineStageFlags StageFlags = 0;
	if (in_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		StageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	if (in_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		StageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	if (in_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		StageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	if (in_layout == VK_IMAGE_LAYOUT_GENERAL) {
		StageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT;
	}


	const VkSubmitInfo SubmitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = nullptr,
		.pWaitDstStageMask = nullptr,
		.commandBufferCount = 1,
		.pCommandBuffers = &m_presentCommandSet,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = nullptr
	};

	if(in_stage == nullptr) {
		const VkImageMemoryBarrier UnToLayoutBarrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = 0,
			.dstAccessMask = AccessFlags,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = in_layout,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = in_texture->image,
			.subresourceRange = {
				.aspectMask = static_cast<VkImageAspectFlags>(in_flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT),
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
		};

		CHECK(vkBeginCommandBuffer(m_presentCommandSet, &G_FIXED_COMMAND_BEGIN_INFO))
		vkCmdPipelineBarrier(m_presentCommandSet, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, StageFlags, 0, 0, nullptr, 0, nullptr, 1, &UnToLayoutBarrier);
		vkEndCommandBuffer(m_presentCommandSet);
		CHECK(vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &SubmitInfo, VK_NULL_HANDLE))
		CHECK(vkQueueWaitIdle(m_deferredRenderingCommandQueue))
	} else {
		vkUnmapMemory(m_logical, in_stage->memory);

		const VkBufferImageCopy Region = {
			.bufferOffset = 0,
			.bufferRowLength = 0,
			.bufferImageHeight = 0,
			.imageSubresource = {
				.aspectMask = static_cast<VkImageAspectFlags>(in_flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT),
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
				.imageOffset = { 0 },
				.imageExtent = in_extent
		};
		const VkImageMemoryBarrier UnToDstBarrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = in_texture->image,
			.subresourceRange = {
				.aspectMask = static_cast<VkImageAspectFlags>(in_flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT),
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
		};

		const VkImageMemoryBarrier DstToLayoutBarrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.dstAccessMask = AccessFlags,
			.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.newLayout = in_layout,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = in_texture->image,
			.subresourceRange = {
				.aspectMask = static_cast<VkImageAspectFlags>(in_flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT),
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
		};

		CHECK(vkBeginCommandBuffer(m_presentCommandSet, &G_FIXED_COMMAND_BEGIN_INFO))
		vkCmdPipelineBarrier(m_presentCommandSet, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &UnToDstBarrier);
		vkCmdCopyBufferToImage(m_presentCommandSet, in_stage->buffer, in_texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &Region);
		vkCmdPipelineBarrier(m_presentCommandSet, VK_PIPELINE_STAGE_TRANSFER_BIT, StageFlags, 0, 0, nullptr, 0, nullptr, 1, &DstToLayoutBarrier);
		vkEndCommandBuffer(m_presentCommandSet);
		CHECK(vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &SubmitInfo, VK_NULL_HANDLE))
		CHECK(vkQueueWaitIdle(m_deferredRenderingCommandQueue))
		vkFreeMemory(m_logical, in_stage->memory, nullptr);
		vkDestroyBuffer(m_logical, in_stage->buffer, nullptr);
	}
	
	
}

void GPUFixedContext::ruin_localTexture(GPULocalTexture* in_texture) {
	vkDestroyImageView(m_logical, in_texture->view, nullptr);
	vkFreeMemory(m_logical, in_texture->memory, nullptr);
	vkDestroyImage(m_logical, in_texture->image, nullptr);
}



#endif
