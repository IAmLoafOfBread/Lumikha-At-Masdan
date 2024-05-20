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
	GPUFixedContext(GLFWwindow* in_surfaceWindow, GPUExtent3D in_surfaceExtent, const uint32_t in_meshCount, const uint32_t* in_meshInstanceMaxCounts, const char** in_positionFiles, const char** in_normalFiles, const char** in_uvFiles, const char** in_indexFiles, const char*** in_textureFiles);
	~GPUFixedContext();
	
private:
	GLFWwindow* m_surfaceWindow = nullptr;
	
	uint32_t m_localMemoryIndex = 0;
	uint32_t m_sharedMemoryIndex = 0;
	GPUDevice m_logical = GPU_NULL_HANDLE;
	
	uint32_t m_graphicsQueueFamilyIndex = 0;
	bool m_multiThreadedGraphics = true;

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
	
	GPURenderPass m_deferredRenderingPass = GPU_NULL_HANDLE;
	
	GPURenderPass m_shadowMappingPasses[CASCADED_SHADOW_MAP_COUNT] = { GPU_NULL_HANDLE };
	
	GPUSwapchain m_swapchain = GPU_NULL_HANDLE;
	GPUTextureView* m_presentViews = nullptr;
	
	GPULocalTexture m_geometryOutputAttachments[GEOMETRY_PASS_OUTPUT_COUNT] = { { GPU_NULL_HANDLE } };
	GPUFramebuffer* m_deferredRenderingFramebuffers = nullptr;
	
	GPULocalTexture m_shadowMappingAttachments[CASCADED_SHADOW_MAP_COUNT][MAX_LIGHT_COUNT] = { { { GPU_NULL_HANDLE } } };
	GPUFramebuffer m_shadowMappingFramebuffers[CASCADED_SHADOW_MAP_COUNT][MAX_LIGHT_COUNT] = { { GPU_NULL_HANDLE } };
	
	uint32_t m_meshCount = 0;
	GPUDescriptorLayout m_deferredRenderingDescriptorLayout = GPU_NULL_HANDLE;
	GPUDescriptorSet m_deferredRenderingDescriptorSet = GPU_NULL_HANDLE;
	
	GPUGraphicsLayout m_deferredRenderingLayout = GPU_NULL_HANDLE;
	GPUGraphicsPipeline m_deferredRenderingPipelines[2] = { GPU_NULL_HANDLE };
	
	GPUGraphicsLayout m_shadowMappingLayout = GPU_NULL_HANDLE;
	GPUGraphicsPipeline m_shadowMappingPipelines[CASCADED_SHADOW_MAP_COUNT] = { GPU_NULL_HANDLE };
	
	GPUBuffer m_graphicsVertexBuffer = GPU_NULL_HANDLE;
	GPUIndirectDrawCommand* m_graphicsIndirectCommands = nullptr;
	GPUBuffer m_graphicsIndirectCommandBuffer = GPU_NULL_HANDLE;
	GPUBuffer m_graphicsInstanceBuffer = GPU_NULL_HANDLE;
	GPUTextureView* m_graphicsMeshTextureViews[DEFERRED_RENDERING_ATTACHMENT_COUNT] = { nullptr };
	
	uint32_t m_lightCount = 0;
	GPUBuffer m_graphicsLightBuffer = GPU_NULL_HANDLE;
	Light* m_lights = nullptr;
	
	View* m_cameraView = nullptr;
	
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
	
	void build_deferredRenderingPass(void);
	void ruin_deferredRenderingPass(void);
	
	void build_shadowMappingPasses(void);
	void ruin_shadowMappingPasses(void);
	
	void build_swapchain(void);
	void ruin_swapchain(void);
	
	void build_deferredRenderingFramebuffers(void);
	void ruin_deferredRenderingFramebuffers(void);
	
	void build_shadowMappingFramebuffers(void);
	void ruin_shadowMappingFramebuffers(void);
	
	void build_deferredRenderingBindings(uint32_t in_meshCount);
	void ruin_deferredRenderingBindings(void);
	
	void build_deferredRenderingPipelines(void);
	void ruin_deferredRenderingPipelines(void);
	
	void build_shadowMappingPipelines(void);
	void ruin_shadowMappingPipelines(void);
	
	void build_meshes(uint32_t* in_vertexCounts, GPUStageAllocation* in_vertexAllocation, const uint32_t* in_maxInstanceCounts, GPUStageAllocation** in_textureAllocations, GPUExtent3D** in_extents);
	void ruin_meshes(void);
	
	void build_lights(void);
	void ruin_lights(void);
	
	void set_deferredRenderingBindings(void);
	void unset_deferredRenderingBindings(void);
	
	void run_shadowMaps(uint32_t in_index, uint32_t in_divisor);
	void run_deferredRenders(float3** in_viewPosition, float3** in_viewRotation, Instance** in_instances, Light** in_lights, void(*in_startupCallback)(void));
	
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
