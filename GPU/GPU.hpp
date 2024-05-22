#ifndef GPU_TYPES_H
#define GPU_TYPES_H

#include <math.h>

#include "../Threads/Threads.h"
#include "../Files/Files.h"
#include "Shader-Utilities.h"

#include "Vulkan-Context/Vulkan-Definitions.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spng.h>



class GPUFixedContext {
public:
	GPUFixedContext(GLFWwindow* in_surfaceWindow, GPUExtent3D in_surfaceExtent, const uint32_t in_meshCount, const uint32_t* in_meshInstanceMaxCounts, const char** in_positionFiles, const char** in_normalFiles, const char** in_uvFiles, const char** in_indexFiles, const char*** in_textureFiles, void(*in_startupCallback)(void*));
	~GPUFixedContext();

	Camera* m_camera = nullptr;
	Instance* m_instances = nullptr;
	Light* m_lights = nullptr;
	
private:
	GLFWwindow* m_surfaceWindow = nullptr;
	
	uint32_t m_localMemoryIndex = 0;
	uint32_t m_sharedMemoryIndex = 0;
	GPUDevice m_logical = GPU_NULL_HANDLE;
	
	uint32_t m_graphicsQueueFamilyIndex = 0;
	bool m_multiThreadedGraphics = false;

	GPUCommandQueue m_deferredRenderingCommandQueue = GPU_NULL_HANDLE;
	GPUCommandPool m_deferredRenderingCommandPool = GPU_NULL_HANDLE;
	GPUCommandSet m_deferredRenderingCommandSet = GPU_NULL_HANDLE;
	
	GPUCommandQueue m_shadowMappingCommandQueues[CASCADED_SHADOW_MAP_COUNT] = { GPU_NULL_HANDLE };
	GPUCommandPool m_shadowMappingCommandPools[CASCADED_SHADOW_MAP_COUNT] = { GPU_NULL_HANDLE };
	GPUCommandSet m_shadowMappingCommandSets[CASCADED_SHADOW_MAP_COUNT] = { GPU_NULL_HANDLE };
	
	GPUSurface m_surface = GPU_NULL_HANDLE;
	uint32_t m_surfaceFrameCount = 0;
	GPUExtent3D m_surfaceExtent = { 0 };
	GPUFormat m_surfaceFormat = static_cast<GPUFormat>(0);
	GPUColourSpace m_surfaceColourSpace = static_cast<GPUColourSpace>(0);
	
	GPURenderPass m_shadowMappingPass = GPU_NULL_HANDLE;
	
	GPURenderPass m_geometryPass = GPU_NULL_HANDLE;
	
	GPURenderPass m_lightingPass = GPU_NULL_HANDLE;
	
	GPUSwapchain m_swapchain = GPU_NULL_HANDLE;
	GPUTextureView* m_presentViews = nullptr;
	
	GPUTextureView m_shadowMappingViews[CASCADED_SHADOW_MAP_COUNT][MAX_LIGHT_COUNT] = { { { GPU_NULL_HANDLE } } };
	GPUFramebuffer m_shadowMappingFramebuffers[CASCADED_SHADOW_MAP_COUNT][MAX_LIGHT_COUNT] = { { GPU_NULL_HANDLE } };
	
	GPUTextureView m_geometryViews[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT] = { { GPU_NULL_HANDLE } };
	GPUFramebuffer m_geometryFramebuffer = GPU_NULL_HANDLE;

	GPUFramebuffer* m_lightingFramebuffers = nullptr;
	
	uint32_t m_meshCount = 0;
	GPUDescriptorPool m_graphicsBindingPool = GPU_NULL_HANDLE;

	GPUDescriptorLayout m_geometryDescriptorLayout = GPU_NULL_HANDLE;
	GPUDescriptorSet m_geometryDescriptorSet = GPU_NULL_HANDLE;

	GPUDescriptorLayout m_lightingDescriptorLayout = GPU_NULL_HANDLE;
	GPUDescriptorSet m_lightingDescriptorSet = GPU_NULL_HANDLE;
	
	GPUGraphicsLayout m_shadowMappingLayout = GPU_NULL_HANDLE;
	GPUGraphicsPipeline m_shadowMappingPipelines[CASCADED_SHADOW_MAP_COUNT] = { GPU_NULL_HANDLE };

	GPUGraphicsLayout m_geometryLayout = GPU_NULL_HANDLE;
	GPUGraphicsPipeline m_geometryPipeline = GPU_NULL_HANDLE;

	GPUGraphicsLayout m_lightingLayout = GPU_NULL_HANDLE;
	GPUGraphicsPipeline m_lightingPipeline = GPU_NULL_HANDLE;
	
	GPUBuffer m_graphicsMeshBuffer = GPU_NULL_HANDLE;
	GPUIndirectDrawCommand* m_graphicsIndirectCommands = nullptr;
	GPUBuffer m_graphicsIndirectCommandBuffer = GPU_NULL_HANDLE;
	GPUBuffer m_graphicsInstanceBuffer = GPU_NULL_HANDLE;
	GPUTextureView* m_graphicsMeshTextureViews[GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT] = { nullptr };
	
	uint32_t m_lightCount = 0;
	GPUBuffer m_graphicsLightBuffer = GPU_NULL_HANDLE;
	Light* m_lights = nullptr;
	
	GPUSampler m_sampler = GPU_NULL_HANDLE;
	
	void set_meshes(GPUStageAllocation* in_allocation, uint32_t in_meshCount, const char** in_positionFiles, const char** in_normalFiles, const char** in_uvFiles, const char** in_indexFiles, uint32_t* in_vertexCounts);
	void set_texture(GPUStageAllocation* in_allocation, const char* in_file, GPUExtent3D* in_extent);
	
	void build_stageAllocation(GPUStageAllocation* in_stage, GPUSize in_size);
	void build_sharedAllocation(GPUSharedAllocation* in_shared, GPUSize in_size, GPUBufferUsageFlags in_flags, bool in_computeIndexInclude);
	void ruin_sharedAllocation(GPUSharedAllocation* in_shared);
	void build_localAllocation(GPULocalAllocation* in_local, GPUStageAllocation* in_stage, GPUBufferUsageFlags in_flags);
	void ruin_localAllocation(GPULocalAllocation* in_local);
	void build_localTexture(GPULocalTexture* in_texture, GPUStageAllocation* in_stage, GPUFormat in_format, GPUExtent3D in_extent, GPUImageUsageFlags in_flags);
	void ruin_localTexture(GPULocalTexture* in_texture);
	
	void build_module(GPUModule in_module, const char* in_path);
	void ruin_module(GPUModule in_module);
	
	void build_commandThread(uint32_t in_queueFamilyIndex, uint32_t in_queueIndex, GPUCommandQueue* in_queue, GPUCommandPool* in_pool, GPUCommandSet* in_set);
	void ruin_commandThread(GPUCommandPool* in_pool, GPUCommandSet* in_set);
	void build_device(GLFWwindow* in_window, GPUExtent3D in_extent);
	void ruin_device(void);
	
	void build_shadowMappingPass(void);
	void ruin_shadowMappingPass(void);
	
	void build_geometryPass(void);
	void ruin_geometryPass(void);

	void build_lightingPass(void);
	void ruin_lightingPass(void);
	
	void build_swapchain(void);
	void ruin_swapchain(void);
	
	void build_shadowMappingFramebuffers(void);
	void ruin_shadowMappingFramebuffers(void);
	
	void build_geometryFramebuffer(void);
	void ruin_geometryFramebuffer(void);

	void build_lightingFramebuffers(void);
	void ruin_lightingFramebuffers(void);

	void build_graphicsBindingPool(uint32_t in_meshCount);
	void ruin_graphicsBindingPool(void);
	
	void build_geometryBindings(void);
	void ruin_geometryBindings(void);

	void build_lightingBindings(void);
	void ruin_lightingBindings(void);
	
	void build_shadowMappingPipelines(void);
	void ruin_shadowMappingPipelines(void);

	void build_geometryPipeline(void);
	void ruin_geometryPipeline(void);

	void build_lightingPipeline(void);
	void ruin_lightingPipeline(void);
	
	void build_meshes(uint32_t* in_vertexCounts, GPUStageAllocation* in_vertexAllocation, const uint32_t* in_maxInstanceCounts, GPUStageAllocation** in_textureAllocations, GPUExtent3D** in_extents);
	void ruin_meshes(void);
	
	void build_lights(void);
	void ruin_lights(void);
	
	void build_sampler(void);
	void ruin_sampler(void);

	void set_geometryBindings(void);

	void set_lightingBindings(void);
	
	void run_shadowMappings(uint32_t in_index, uint32_t in_divisor);
	void run_deferredRenderings(void(*in_startupCallback)(void*), void* in_callbackArgument);
	
	void add_instance(Instance* in_instance);
	void rid_instance(uint32_t);
	
	void add_light(Light* in_light);
	void rid_light(uint32_t);
	
	static void* m_shadowMappingCallback(void* in_info);
};

typedef struct ShadowMappingInfo {
	GPUFixedContext* context;
	uint32_t index;
	uint32_t divisor;
} ShadowMappingInfo;



#endif
