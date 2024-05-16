#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static const VkDeviceSize Offset = 0;
static const uint32_t ViewSize = sizeof(View);
static const uint32_t DrawCommandSize = sizeof(VkDrawIndirectCommand);
static VkSemaphore g_shadowsFinishedSemaphores[CASCADED_SHADOW_MAP_COUNT] = { VK_NULL_HANDLE };
static VkSemaphore g_imageAvailableSemaphore = VK_NULL_HANDLE;
static const VkCommandBufferBeginInfo g_commandInfo = {
	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	.pNext = nullptr,
	.flags = 0,
	.pInheritanceInfo = nullptr
};



void GPUFixedContext::run_shadowMaps(uint32_t in_index, uint32_t in_divisor) {
	VkPipeline Pipeline = m_shadowMappingPipelines[in_index];
	VkFramebuffer* Framebuffers = m_shadowMappingFramebuffers[in_index];
	VkSemaphore Semaphore = g_shadowsFinishedSemaphores[in_index];
	VkCommandBuffer Set = m_shadowMappingCommandSets[in_index];
	VkQueue Queue = m_shadowMappingCommandQueues[in_index];
	
	VkImageMemoryBarrier DepthToReadBarriers[MAX_LIGHT_COUNT] = {  };
	for(uint32_t i = 0; i < MAX_LIGHT_COUNT; i++) {
		DepthToReadBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		DepthToReadBarriers[i].pNext = nullptr;
		DepthToReadBarriers[i].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		DepthToReadBarriers[i].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		DepthToReadBarriers[i].oldLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		DepthToReadBarriers[i].newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
		DepthToReadBarriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		DepthToReadBarriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		DepthToReadBarriers[i].image = m_shadowMappingAttachments[in_index][i].image;
		DepthToReadBarriers[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		DepthToReadBarriers[i].subresourceRange.baseMipLevel = 0;
		DepthToReadBarriers[i].subresourceRange.levelCount = 1;
		DepthToReadBarriers[i].subresourceRange.baseArrayLayer = 0;
		DepthToReadBarriers[i].subresourceRange.layerCount = 1;
	}
	
	const VkClearValue ClearValue = {
		.depthStencil = {
			.depth = 0
		}
	};
	const VkExtent3D Extent = SHADOW_MAP_EXTENT;
	VkRenderPassBeginInfo RenderInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = m_shadowMappingPasses[in_index],
		.framebuffer = VK_NULL_HANDLE,
		.renderArea = {
			.offset = { 0 },
			.extent = {Extent.width / in_divisor, Extent.height / in_divisor},
		},
		.clearValueCount = 1,
		.pClearValues = &ClearValue
	};
	
	VkPipelineStageFlags WaitStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	const VkSubmitInfo SubmitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &g_imageAvailableSemaphore,
		.pWaitDstStageMask = &WaitStage,
		.commandBufferCount = 1,
		.pCommandBuffers = &m_deferredRenderingCommandSet,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &Semaphore
	};
	
	while(!glfwWindowShouldClose(m_surfaceWindow)) {
		glfwPollEvents();
		vkBeginCommandBuffer(Set, &g_commandInfo);
		vkCmdBindPipeline(Set, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);
		vkCmdPushConstants(Set, m_shadowMappingLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, ViewSize, m_cameraView);
		vkCmdBindVertexBuffers(Set, 0, 1, &m_graphicsVertexBuffer, &Offset);

		for(uint32_t i = 0; i < m_lightCount; i++) {
			RenderInfo.framebuffer = Framebuffers[i];
			vkCmdPushConstants(Set, m_shadowMappingLayout, VK_SHADER_STAGE_VERTEX_BIT, ViewSize, ViewSize, &m_lights[i]);
			vkCmdBeginRenderPass(Set, &RenderInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdDrawIndirect(Set, m_graphicsIndirectCommandBuffer, 0, m_meshCount, DrawCommandSize);
			vkCmdEndRenderPass(Set);
		}
		
		vkCmdPipelineBarrier(Set, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, m_lightCount, DepthToReadBarriers);
		vkEndCommandBuffer(Set);
		vkQueueSubmit(Queue, 1, &SubmitInfo, VK_NULL_HANDLE);
	}
	vkQueueWaitIdle(Queue);
}

void GPUFixedContext::run_deferredRenders(float3** in_viewPosition, float3** in_viewRotation, Instance** in_instances, Light** in_lights, void(*in_startupCallback)(void)) {
	VkSemaphore RenderFinishedSemaphore = VK_NULL_HANDLE; {
		const VkSemaphoreCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0
		};
		CHECK(vkCreateSemaphore(m_logical, &CreateInfo, nullptr, &RenderFinishedSemaphore))
		CHECK(vkCreateSemaphore(m_logical, &CreateInfo, nullptr, &g_imageAvailableSemaphore))
		for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
			CHECK(vkCreateSemaphore(m_logical, &CreateInfo, nullptr, &g_shadowsFinishedSemaphores[i]))
		}
	}
	
	VkImageMemoryBarrier ColourToReadBarriers[GEOMETRY_PASS_OUTPUT_COUNT] = {  };
	for(uint32_t i = 0; i < GEOMETRY_PASS_OUTPUT_COUNT; i++) {
		ColourToReadBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		ColourToReadBarriers[i].pNext = nullptr;
		ColourToReadBarriers[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		ColourToReadBarriers[i].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		ColourToReadBarriers[i].oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		ColourToReadBarriers[i].newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
		ColourToReadBarriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		ColourToReadBarriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		ColourToReadBarriers[i].image = m_geometryOutputAttachments[i].image;
		ColourToReadBarriers[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ColourToReadBarriers[i].subresourceRange.baseMipLevel = 0;
		ColourToReadBarriers[i].subresourceRange.levelCount = 1;
		ColourToReadBarriers[i].subresourceRange.baseArrayLayer = 0;
		ColourToReadBarriers[i].subresourceRange.layerCount = 1;
	}
	
	VkClearValue ClearValues[DEFERRED_RENDERING_ATTACHMENT_COUNT];
	for(uint32_t i = 0; i < DEFERRED_RENDERING_ATTACHMENT_COUNT; i++) {
		if(i != DEFERRED_RENDERING_ATTACHMENT_DEPTH_INDEX) {
			ClearValues[i].color.float32[0] = 0;
			ClearValues[i].color.float32[1] = 0;
			ClearValues[i].color.float32[2] = 0;
			ClearValues[i].color.float32[3] = 1;
		} else ClearValues[i].depthStencil.depth = 0;
	}
	VkRenderPassBeginInfo RenderInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = m_deferredRenderingPass,
		.framebuffer = VK_NULL_HANDLE,
		.renderArea = {
			.offset = { 0 },
			.extent = {m_surfaceExtent.width, m_surfaceExtent.height},
		},
		.clearValueCount = DEFERRED_RENDERING_ATTACHMENT_COUNT,
		.pClearValues = ClearValues
	};
	
	const uint32_t TotalWaitCount = CASCADED_SHADOW_MAP_COUNT + 1;
	VkPipelineStageFlags WaitStages[TotalWaitCount] = { VK_PIPELINE_STAGE_NONE };
	VkSemaphore WaitSemaphores[TotalWaitCount] = { VK_NULL_HANDLE };
	for(uint32_t i = 0; i < TotalWaitCount; i++) WaitStages[i] = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) WaitSemaphores[i] = g_shadowsFinishedSemaphores[i];
	WaitSemaphores[CASCADED_SHADOW_MAP_COUNT] = g_imageAvailableSemaphore;
	const VkSubmitInfo SubmitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = TotalWaitCount,
		.pWaitSemaphores = WaitSemaphores,
		.pWaitDstStageMask = WaitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = &m_deferredRenderingCommandSet,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &RenderFinishedSemaphore
	};
	
	uint32_t ImageIndex = 0;
	const VkPresentInfoKHR PresentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &RenderFinishedSemaphore,
		.swapchainCount = 1,
		.pSwapchains = &m_swapchain,
		.pImageIndices = &ImageIndex,
		.pResults = nullptr
	};
	
	const float XYFunc = 1.0 / tanf(1.57079 / 2.0);
	const float ZNear = 1000.0;
	const float ZFar = 1.0;
	const float ZFunc = ZFar / (ZFar - ZNear);
	View CameraData = {
		.position = { 0 },
		.rotation = { 0 },
		.projection = {
			.vecs[0] = {static_cast<float>(m_surfaceExtent.height) / static_cast<float>(m_surfaceExtent.width) * XYFunc, 0, 0, 0},
			.vecs[1] = {0, XYFunc, 0, 0},
			.vecs[2] = {0, 0, ZFunc, 1},
			.vecs[3] = {0, 0, -ZFunc * ZNear, 0},
		}
	};
	*in_viewPosition = &CameraData.position;
	*in_viewRotation = &CameraData.rotation;
	m_cameraView = &CameraData;
	uint32_t* LightCount = &m_lightCount;
	
	while(!glfwWindowShouldClose(m_surfaceWindow)) {
		glfwPollEvents();
		vkAcquireNextImageKHR(m_logical, m_swapchain, UINT64_MAX, g_imageAvailableSemaphore, VK_NULL_HANDLE, &ImageIndex);
		RenderInfo.framebuffer = m_deferredRenderingFramebuffers[ImageIndex];
		
		vkBeginCommandBuffer(m_deferredRenderingCommandSet, &g_commandInfo);
		vkCmdBindDescriptorSets(m_deferredRenderingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_deferredRenderingLayout, 0, 1, &m_deferredRenderingDescriptorSet, 0 , nullptr);
		
		vkCmdBindPipeline(m_deferredRenderingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_deferredRenderingPipelines[0]);
		vkCmdPushConstants(m_deferredRenderingCommandSet, m_deferredRenderingLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, ViewSize, m_cameraView);
		vkCmdPushConstants(m_deferredRenderingCommandSet, m_deferredRenderingLayout, VK_SHADER_STAGE_FRAGMENT_BIT, ViewSize, 4, LightCount);
		vkCmdBindVertexBuffers(m_deferredRenderingCommandSet, 0, 1, &m_graphicsVertexBuffer, &Offset);
		vkCmdBeginRenderPass(m_deferredRenderingCommandSet, &RenderInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		vkCmdDrawIndirect(m_deferredRenderingCommandSet, m_graphicsIndirectCommandBuffer, 0, m_meshCount, DrawCommandSize);
		vkCmdPipelineBarrier(m_deferredRenderingCommandSet, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, GEOMETRY_PASS_OUTPUT_COUNT, ColourToReadBarriers);
		
		vkCmdBindPipeline(m_deferredRenderingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_deferredRenderingPipelines[1]);
		vkCmdNextSubpass(m_deferredRenderingCommandSet, VK_SUBPASS_CONTENTS_INLINE);
		
		vkCmdDraw(m_deferredRenderingCommandSet, 4, 1, 0, 0);
		
		vkCmdEndRenderPass(m_deferredRenderingCommandSet);
		
		vkEndCommandBuffer(m_deferredRenderingCommandSet);
		vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &SubmitInfo, VK_NULL_HANDLE);
		vkQueuePresentKHR(m_deferredRenderingCommandQueue, &PresentInfo);
	}
	CHECK(vkQueueWaitIdle(m_deferredRenderingCommandQueue))
	vkDestroySemaphore(m_logical, g_imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(m_logical, RenderFinishedSemaphore, nullptr);
}



#endif
