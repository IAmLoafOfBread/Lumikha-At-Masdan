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
typedef VkImageView GPUTextureView;
typedef VkFramebuffer GPUFramebuffer;
typedef VkDescriptorPool GPUDescriptorPool;
typedef VkDescriptorSetLayout GPUDescriptorLayout;
typedef VkDescriptorSet GPUDescriptorSet;
typedef VkPipeline GPUGraphicsPipeline;
typedef VkPipelineLayout GPUGraphicsLayout;
typedef VkDrawIndirectCommand GPUIndirectDrawCommand;
typedef VkFormat GPUFormat;
typedef VkColorSpaceKHR GPUColourSpace;
typedef VkBufferUsageFlags GPUBufferUsageFlags;
typedef VkImageUsageFlags GPUImageUsageFlags;
typedef VkBuffer GPUBuffer;
typedef struct GPUSharedAllocation {
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceAddress address;
	void* data;
} GPUSharedAllocation;
typedef struct GPULocalAllocation {
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceAddress address;
} GPULocalAllocation;
typedef struct GPULocalTexture {
	VkImage image;
	VkDeviceMemory memory;
	VkImageView view;
} GPULocalTexture;
typedef struct GPUStageAllocation {
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceSize size;
	void* data;
} GPUStageAllocation;



#endif

#endif
