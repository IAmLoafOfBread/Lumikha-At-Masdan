#ifndef VULKAN_DEFINITIONS_H
#define VULKAN_DEFINITIONS_H

#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)



#include <volk.h>
#define GLFW_INCLUDE_VULKAN

#define GPU_NULL_HANDLE VK_NULL_HANDLE

typedef VkDeviceSize GPUSize;
typedef VkExtent2D GPUExtent2D;
typedef VkExtent3D GPUExtent3D;
typedef VkSurfaceKHR GPUSurface;
typedef VkDevice GPUDevice;
typedef VkQueue GPUCommandQueue;
typedef VkCommandPool GPUCommandPool;
typedef VkCommandBuffer GPUCommandSet;
typedef VkShaderModule GPUModule;
typedef VkRenderPass GPURenderPass;
typedef VkSwapchainKHR GPUSwapchain;
typedef VkImage GPUTextureImage;
typedef VkImageView GPUTextureView;
typedef VkFramebuffer GPUFramebuffer;
typedef VkDescriptorPool GPUDescriptorPool;
typedef VkDescriptorSetLayout GPUDescriptorLayout;
typedef VkDescriptorSet GPUDescriptorSet;
typedef VkSampler GPUSampler;
typedef VkPipeline GPUGraphicsPipeline;
typedef VkPipelineLayout GPUGraphicsLayout;
typedef VkPipeline GPUComputePipeline;
typedef VkPipelineLayout GPUComputeLayout;
typedef VkDrawIndirectCommand GPUIndirectDrawCommand;
typedef VkFence GPUFence;
typedef VkSemaphore GPUSemaphore;
typedef VkFormat GPUFormat;
typedef VkColorSpaceKHR GPUColourSpace;
typedef VkBufferUsageFlags GPUBufferUsageFlags;
typedef VkImageUsageFlags GPUImageUsageFlags;
typedef VkImageLayout GPUImageLayout;
typedef VkBuffer GPUBuffer;
struct GPUSharedAllocation {
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceAddress address;
	void* data;
};
struct GPULocalAllocation {
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceAddress address;
};
struct GPULocalTexture {
	VkImage image;
	VkDeviceMemory memory;
	VkImageView view;
};
struct GPUStageAllocation {
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceSize size;
	void* data;
};



const VkCommandBufferBeginInfo G_FIXED_COMMAND_BEGIN_INFO = {
	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	.pNext = nullptr,
	.flags = 0,
	.pInheritanceInfo = nullptr
};
const VkClearValue G_FIXED_DEPTH_CLEAR_VALUE = {
	.depthStencil = {
		.depth = 0
	}
};
const VkPipelineStageFlags G_FIXED_PIPELINE_STAGE_TOP_OF_PIPE_BIT = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;



#endif

#endif
