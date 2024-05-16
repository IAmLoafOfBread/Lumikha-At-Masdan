#include "../../../BuildInfo.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.h"










void run_GPUDraw(fvec3d_st** inViewPosition, fvec3d_st** inViewRotation, inst_st** inInstances, light_st** inLights, void(*inStart)(void)) {
	*inInstances = instances;
	*inLights = lights;

	uint32_t ImageIndex = 0;
	
	VkSemaphore ImageAvailableSemaphore = VK_NULL_HANDLE;
	VkSemaphore RenderFinishedSemaphore = VK_NULL_HANDLE; {
		const VkSemaphoreCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0
		};
		CHECK(vkCreateSemaphore(logical, &CreateInfo, NULL, &ImageAvailableSemaphore))
		CHECK(vkCreateSemaphore(logical, &CreateInfo, NULL, &RenderFinishedSemaphore))
	}
	VkFence InFlightFence = VK_NULL_HANDLE; {
		const VkFenceCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = NULL,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};
		CHECK(vkCreateFence(logical, &CreateInfo, NULL, &InFlightFence))
	}
	const VkClearValue Clears[] = {
		{
			.color = {0.4f, 0.6f, 0.8f, 1.0f},
		},
		{
			.depthStencil = {
				.depth = 0,
				.stencil = 0
			}
		}
	};
	VkRenderPassBeginInfo RenderInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = NULL,
		.renderPass = renderPass,
		.framebuffer = VK_NULL_HANDLE,
		.renderArea = {
			.offset = { 0 },
			.extent = extent
		},
			.clearValueCount = 2,
			.pClearValues = Clears
	};
	const VkCommandBufferBeginInfo CommandInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = 0,
		.pInheritanceInfo = NULL
	};
	const VkPipelineStageFlags PipeStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	const VkSubmitInfo SubmitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &ImageAvailableSemaphore,
		.pWaitDstStageMask = &PipeStageFlag,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandSet,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &RenderFinishedSemaphore
	};
	VkPresentInfoKHR PresentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &RenderFinishedSemaphore,
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &ImageIndex,
		.pResults = NULL
	};
	const uint32_t CommandStride = sizeof(VkDrawIndirectCommand);
	VkBuffer CommandBuffer = commandBuffer.buffer;
	VkBuffer VertexBuffer = vertexBuffer.buffer;
	VkBuffer InstanceBuffer = instanceBuffer.buffer;
	const uint32_t VertexRangeSize = sizeof(struct VertexConstantRange);
	const uint32_t FragmentRangeSize = sizeof(struct FragmentConstantRange);
	struct LocalBuffer ProjBuffer = {};
	{
		const float FOV = 1.57079f;
		const float ZNear = 1000.0f;
		const float ZFar = 1.0f;
		const float XYFunc = 1.0f / tanf(FOV / 2.0f);
		const float ZFunc = ZFar / (ZFar - ZNear);
		const float Width = (float)extent.width;
		const float Height = (float)extent.height;
		struct StageBuffer StageBuffer;
		build_stageBuffer(&StageBuffer, sizeof(fmat4x4_st), NULL);
		fmat4x4_st* ProjMat = (fmat4x4_st*)(StageBuffer.data);
		ProjMat->vecs[0].x = (Height / Width) * XYFunc;
		ProjMat->vecs[0].y = 0;
		ProjMat->vecs[0].z = 0;
		ProjMat->vecs[0].w = 0;
		ProjMat->vecs[1].x = 0;
		ProjMat->vecs[1].y = XYFunc;
		ProjMat->vecs[1].z = 0;
		ProjMat->vecs[1].w = 0;
		ProjMat->vecs[2].x = 0;
		ProjMat->vecs[2].y = 0;
		ProjMat->vecs[2].z = ZFunc;
		ProjMat->vecs[2].w = 1;
		ProjMat->vecs[3].x = 0;
		ProjMat->vecs[3].y = 0;
		ProjMat->vecs[3].z = -ZFunc * ZNear;
		ProjMat->vecs[3].w = 0;
		build_localBuffer(&ProjBuffer, &StageBuffer, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
	}
	struct VertexConstantRange VertexConstantBuffer = {
		.viewPos = { 0 },
		.viewRot = { 0 },
		.proj = ProjBuffer.address
	};
	struct VertexConstantRange* VertexRange = &VertexConstantBuffer;
	
	*inViewPosition = &VertexConstantBuffer.viewPos;
	*inViewRotation = &VertexConstantBuffer.viewRot;
	
	struct FragmentConstantRange FragmentConstantBuffer = {
		.lightCount = 0,
		.lights = lightBuffer.address
	};
	struct FragmentConstantRange* FragmentRange = &FragmentConstantBuffer;
	
	VkDeviceSize Offset = 0;
	GLFWwindow* Window = window;
	inStart();
	Thread MeshThread;
	Thread LightThread;
	create_thread(&MeshThread, &update_meshes, NULL);
	create_thread(&LightThread, &update_lights, NULL);
	while(!glfwWindowShouldClose(Window)) {
		glfwPollEvents();

		vkAcquireNextImageKHR(logical, swapchain, UINT64_MAX, ImageAvailableSemaphore, VK_NULL_HANDLE, &ImageIndex);
		RenderInfo.framebuffer = framebuffers[ImageIndex];
		vkWaitForFences(logical, 1, &InFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(logical, 1, &InFlightFence);
		wait_semaphore(meshSemaphore);
		wait_semaphore(lightSemaphore);
		wait_semaphore(viewSemaphore);
		vkBeginCommandBuffer(commandSet, &CommandInfo);
		vkCmdBeginRenderPass(commandSet, &RenderInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		vkCmdPushConstants(commandSet, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, VertexRangeSize, VertexRange);
		vkCmdPushConstants(commandSet, layout, VK_SHADER_STAGE_FRAGMENT_BIT, VertexRangeSize, FragmentRangeSize, FragmentRange);
		vkCmdBindDescriptorSets(commandSet, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0 , 1, &descriptorSet, 0, NULL);
		vkCmdBindVertexBuffers(commandSet, 0, 1, &VertexBuffer, &Offset);
		vkCmdBindVertexBuffers(commandSet, 1, 1, &InstanceBuffer, &Offset);
		vkCmdDrawIndirect(commandSet, CommandBuffer, 0, meshCount, CommandStride);
		vkCmdEndRenderPass(commandSet);
		vkEndCommandBuffer(commandSet);
		signal_semaphore(meshSemaphore);
		signal_semaphore(lightSemaphore);
		signal_semaphore(viewSemaphore);
		
		vkQueueSubmit(queue, 1, &SubmitInfo, InFlightFence);
		vkQueuePresentKHR(queue, &PresentInfo);
	}
	CHECK(vkDeviceWaitIdle(logical))
	await_thread(MeshThread);
	await_thread(LightThread);
	ruin_localBuffer(&ProjBuffer);
	vkDestroyFence(logical, InFlightFence, NULL);
	vkDestroySemaphore(logical, ImageAvailableSemaphore, NULL);
	vkDestroySemaphore(logical, RenderFinishedSemaphore, NULL);
}

void add_GPUInstance(inst_st* inInst, uint32_t inType) {
	uint32_t Offset = instanceOffsets[inType];
	uint32_t* Count = &commands[inType].instanceCount;
	wait_semaphore(meshSemaphore);
	instances[*Count + Offset] = *inInst;
	(*Count)++;
	signal_semaphore(meshSemaphore);
}
void rid_GPUInstance(uint32_t inType, uint32_t inIndex) {
	uint32_t Offset = instanceOffsets[inType];
	uint32_t* Count = &commands[inType].instanceCount;
	wait_semaphore(meshSemaphore);
	(*Count)--;
	instances[inIndex + Offset] = instances[*Count + Offset];
	signal_semaphore(meshSemaphore);
}



void add_GPULight(light_st* inLight) {
	wait_semaphore(lightSemaphore);
	lights[lightCount] = *inLight;
	lightCount++;
	signal_semaphore(lightSemaphore);
}
void rid_GPULight(uint32_t inIndex) {
	wait_semaphore(lightSemaphore);
	lightCount--;
	lights[inIndex] = lights[lightCount];
	signal_semaphore(lightSemaphore);
}

void* update_meshes(void* n) {
	uint32_t Offset = 0;
	uint32_t JOffset = 0;
	uint32_t i = 0;
	uint32_t j = 0;
	double StartTime = 0;
	double DeltaTime = 0;
	double LastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		Offset = 0;
		wait_semaphore(meshSemaphore);
		
		StartTime = glfwGetTime();
		DeltaTime = StartTime - LastTime;
		LastTime = StartTime;
		
		for(i = 0; i < meshCount; i++) {
			for(j = 0; j < commands[i].instanceCount; j++) {
				JOffset = j + Offset;
				instances[JOffset].update(JOffset, DeltaTime);
			}
			Offset += maxInstanceCounts[i];
		}
		signal_semaphore(meshSemaphore);
	}
	return NULL;
}
void* update_lights(void* n) {
	uint32_t i = 0;
	double StartTime = 0;
	double DeltaTime = 0;
	double LastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		wait_semaphore(lightSemaphore);
		for(i = 0; i < lightCount; i++) {
			
			StartTime = glfwGetTime();
			DeltaTime = StartTime - LastTime;
			LastTime = StartTime;

			lights[i].update(i, DeltaTime);
		}
		signal_semaphore(lightSemaphore);
	}
	return NULL;
}

#endif
