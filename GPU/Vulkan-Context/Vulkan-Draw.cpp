#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static const VkDeviceSize g_offsetData = 0;
static const VkDeviceSize* g_offset = &g_offsetData;
static const uint32_t g_viewSize = sizeof(View);
static const uint32_t g_drawCommandSize = sizeof(VkDrawIndirectCommand);

static VkSemaphore g_shadowsFinishedSemaphores[CASCADED_SHADOW_MAP_COUNT] = { VK_NULL_HANDLE };
static VkSemaphore g_imageAvailableSemaphore = VK_NULL_HANDLE;
static const VkSemaphoreCreateInfo g_semaphoreInfo = {
	.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0
};
static const VkCommandBufferBeginInfo g_commandInfo = {
	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	.pNext = nullptr,
	.flags = 0,
	.pInheritanceInfo = nullptr
};
static const VkClearValue g_depthClearValue = {
	.depthStencil = {
		.depth = 0
	}
};



void GPUFixedContext::run_shadowMappings(uint32_t in_index, uint32_t in_divisor) {
	CHECK(vkCreateSemaphore(m_logical, &g_semaphoreInfo, nullptr, &g_shadowsFinishedSemaphores[in_index]))

	VkPipeline Pipeline = m_shadowMappingPipelines[in_index];
	VkFramebuffer* Framebuffers = m_shadowMappingFramebuffers[in_index];
	VkSemaphore Semaphore = g_shadowsFinishedSemaphores[in_index];
	VkCommandBuffer Set = m_shadowMappingCommandSets[in_index];
	VkQueue Queue = m_shadowMappingCommandQueues[in_index];
	
	const VkExtent3D Extent = SHADOW_MAP_EXTENT;
	VkRenderPassBeginInfo RenderInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = m_shadowMappingPass,
		.framebuffer = VK_NULL_HANDLE,
		.renderArea = {
			.offset = { 0 },
			.extent = {Extent.width / in_divisor, Extent.height / in_divisor},
		},
		.clearValueCount = 1,
		.pClearValues = &g_depthClearValue
	};
	
	VkPipelineStageFlags WaitStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	const VkSubmitInfo SubmitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &g_imageAvailableSemaphore,
		.pWaitDstStageMask = &WaitStage,
		.commandBufferCount = 1,
		.pCommandBuffers = &Set,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &Semaphore
	};
	
	View LightViewData = {
		.position = { 0 },
		.rotation = { 0 },
		.projection = {
			.vecs = { // DOO THIS NOW aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
			}
		}
	};
	View* LightView = &LightViewData;

	const uint32_t CameraSize = sizeof(Camera);

	while(!glfwWindowShouldClose(m_surfaceWindow)) {
		glfwPollEvents();
		vkBeginCommandBuffer(Set, &g_commandInfo);
		vkCmdBindPipeline(Set, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);
		vkCmdPushConstants(Set, m_shadowMappingLayout, VK_SHADER_STAGE_VERTEX_BIT, g_viewSize, CameraSize, m_camera);
		vkCmdBindVertexBuffers(Set, 0, 1, &m_graphicsMeshBuffer, g_offset);
		vkCmdBindVertexBuffers(Set, 1, 1, &m_graphicsInstanceBuffer, g_offset);

		for(uint32_t i = 0; i < m_lightCount; i++) {
			LightViewData.position = m_lights[i].position;
			LightViewData.rotation = m_lights[i].rotation;
			RenderInfo.framebuffer = Framebuffers[i];
			vkCmdPushConstants(Set, m_shadowMappingLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, g_viewSize, LightView);
			vkCmdBeginRenderPass(Set, &RenderInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdDrawIndirect(Set, m_graphicsIndirectCommandBuffer, 0, m_meshCount, g_drawCommandSize);
			vkCmdEndRenderPass(Set);
		}
		
		vkEndCommandBuffer(Set);
		vkQueueSubmit(Queue, 1, &SubmitInfo, VK_NULL_HANDLE);
	}

	vkQueueWaitIdle(Queue);
}

void GPUFixedContext::run_deferredRenderings(void(*in_startupCallback)(void*), void* in_callbackArgument) {
	VkSemaphore RenderFinishedSemaphore = VK_NULL_HANDLE;
	CHECK(vkCreateSemaphore(m_logical, &g_semaphoreInfo, nullptr, &RenderFinishedSemaphore))
	CHECK(vkCreateSemaphore(m_logical, &g_semaphoreInfo, nullptr, &g_imageAvailableSemaphore))
	
	VkClearValue ClearValues[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT + 1];
	for(uint32_t i = 0; i < GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT; i++) {
		ClearValues[i].color.float32[0] = 0;
		ClearValues[i].color.float32[1] = 0;
		ClearValues[i].color.float32[2] = 0;
		ClearValues[i].color.float32[3] = 1;
	}
	ClearValues[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT].depthStencil = g_depthClearValue.depthStencil;

	const VkRenderPassBeginInfo GeometryRenderInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = m_geometryPass,
		.framebuffer = m_geometryFramebuffer,
		.renderArea = {
			.offset = { 0 },
			.extent = {m_surfaceExtent.width, m_surfaceExtent.height},
		},
		.clearValueCount = GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT + 1,
		.pClearValues = ClearValues
	};
	VkRenderPassBeginInfo LightingRenderInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = m_lightingPass,
		.framebuffer = VK_NULL_HANDLE,
		.renderArea = {
			.offset = { 0 },
			.extent = {m_surfaceExtent.width, m_surfaceExtent.height},
		},
		.clearValueCount = 1,
		.pClearValues = ClearValues
	};
	
	VkPipelineStageFlags WaitStages[CASCADED_SHADOW_MAP_COUNT] = { VK_PIPELINE_STAGE_NONE };
	VkSemaphore WaitSemaphores[CASCADED_SHADOW_MAP_COUNT] = { VK_NULL_HANDLE };
	for (uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		WaitStages[i] = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		WaitSemaphores[i] = g_shadowsFinishedSemaphores[i];
	}

	const VkSubmitInfo GeometrySubmitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &g_imageAvailableSemaphore,
		.pWaitDstStageMask = WaitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = &m_deferredRenderingCommandSet,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = nullptr
	};
	const VkSubmitInfo LightingSubmitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = nullptr,
		.pWaitDstStageMask = nullptr,
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
	
	Camera CameraData = {
		.position = { 0 },
		.rotation = { 0 },
		.fov = 1.57,
		.zNear = 1000,
		.zFar = 1
	};
	m_camera = &CameraData;

	const float XYFunc = 1.0 / tanf(CameraData.fov / 2.0);
	const float ZFunc = CameraData.zFar / (CameraData.zFar - CameraData.zNear);
	View ViewData = {
		.position = { 0 },
		.rotation = { 0 },
		.projection = {
			.vecs = {
				{static_cast<float>(m_surfaceExtent.height) / static_cast<float>(m_surfaceExtent.width) * XYFunc, 0, 0, 0},
				{0, XYFunc, 0, 0},
				{0, 0, ZFunc, 1},
				{0, 0, -ZFunc * CameraData.zNear, 0}
			}
		}
	};
	View* View = &ViewData;

	uint32_t* LightCount = &m_lightCount;

	const uint32_t UintSize = sizeof(uint32_t);

	in_startupCallback(in_callbackArgument);
	
	while(!glfwWindowShouldClose(m_surfaceWindow)) {
		glfwPollEvents();
		vkAcquireNextImageKHR(m_logical, m_swapchain, UINT64_MAX, g_imageAvailableSemaphore, VK_NULL_HANDLE, &ImageIndex);
		LightingRenderInfo.framebuffer = m_lightingFramebuffers[ImageIndex];
		
		vkBeginCommandBuffer(m_deferredRenderingCommandSet, &g_commandInfo);
		vkCmdBindDescriptorSets(m_deferredRenderingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_geometryLayout, 0, 1, &m_geometryDescriptorSet, 0 , nullptr);
		vkCmdBindPipeline(m_deferredRenderingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_geometryPipeline);
		vkCmdPushConstants(m_deferredRenderingCommandSet, m_geometryLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, g_viewSize, View);
		vkCmdBindVertexBuffers(m_deferredRenderingCommandSet, 0, 1, &m_graphicsMeshBuffer, g_offset);
		vkCmdBindVertexBuffers(m_deferredRenderingCommandSet, 1, 1, &m_graphicsInstanceBuffer, g_offset);
		
		vkCmdBeginRenderPass(m_deferredRenderingCommandSet, &GeometryRenderInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdDrawIndirect(m_deferredRenderingCommandSet, m_graphicsIndirectCommandBuffer, 0, m_meshCount, g_drawCommandSize);
		vkCmdEndRenderPass(m_deferredRenderingCommandSet);
		
		vkEndCommandBuffer(m_deferredRenderingCommandSet);
		vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &GeometrySubmitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_deferredRenderingCommandQueue);



		vkBeginCommandBuffer(m_deferredRenderingCommandSet, &g_commandInfo);
		vkCmdBindDescriptorSets(m_deferredRenderingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_lightingLayout, 0, 1, &m_lightingDescriptorSet, 0, nullptr);
		vkCmdBindPipeline(m_deferredRenderingCommandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, m_lightingPipeline);
		vkCmdPushConstants(m_deferredRenderingCommandSet, m_lightingLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, UintSize, LightCount);
		
		vkCmdBeginRenderPass(m_deferredRenderingCommandSet, &LightingRenderInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdDraw(m_deferredRenderingCommandSet, 4, 1, 0, 0);
		vkCmdEndRenderPass(m_deferredRenderingCommandSet);

		vkEndCommandBuffer(m_deferredRenderingCommandSet);
		vkQueueSubmit(m_deferredRenderingCommandQueue, 1, &LightingSubmitInfo, VK_NULL_HANDLE);
		vkQueuePresentKHR(m_deferredRenderingCommandQueue, &PresentInfo);
	}

	CHECK(vkQueueWaitIdle(m_deferredRenderingCommandQueue))
	vkDestroySemaphore(m_logical, g_imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(m_logical, RenderFinishedSemaphore, nullptr);
}



#endif
