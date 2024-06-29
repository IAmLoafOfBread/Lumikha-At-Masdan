#ifndef GPU_TYPES_H
#define GPU_TYPES_H



#include <math.h>

#include "../Threads/Threads.h"
#include "../Files/Files.h"
#include "Shader-Utilities.hpp"

#include "Vulkan-Context/Vulkan-Definitions.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spng.h>



class GPUFixedContext {
public:
	GPUFixedContext(GLFWwindow* in_surfaceWindow, GPUExtent3D in_surfaceExtent, const uint32_t in_meshCount, const uint32_t* in_instanceMaxCounts, const char** in_positionFiles, const char** in_normalFiles, const char** in_uvFiles, const char** in_indexFiles, const char*** in_textureFiles);
	~GPUFixedContext();

	bool m_multiThreadedGraphics = false;
	
	Instance* m_instances = nullptr;
	Light* m_lights = nullptr;
	
	Camera m_cameraData = {
		.fov = 1.5707,
		.near = 0.1,
		.far = 100
	};
	
	void acquire_nextImage(void);
	
	void initialize_lightViewingUpdateData(void);
	void dispatch_lightViewingUpdate(void);
	
	void initialize_shadowMappingUpdateData(uint32_t in_index, uint32_t in_divisor);
	void draw_shadowMappingUpdate(uint32_t in_index, uint32_t in_divisor);
	
	void initialize_geometryUpdateData(void);
	void draw_geometryUpdate(void);
	
	void initialize_lightingUpdateData(void);
	void draw_lightingUpdate(void);
	
	void initialize_presentUpdateData(void);
	void submit_presentUpdate(void);
	
	void transform_camera(Transform in_transform, float3 in_value);
	void update_camera(void);
	
private:
	const GPUSize m_fixedOffset = 0;
	uint32_t m_currentImageIndex = 0;
	
	GLFWwindow* m_surfaceWindow = nullptr;
	
	uint32_t m_localMemoryIndex = 0;
	uint32_t m_sharedMemoryIndex = 0;
	GPUDevice m_logical = GPU_NULL_HANDLE;
	
	uint32_t m_graphicsQueueFamilyIndex = NULL_VALUE;
	uint32_t m_computeQueueFamilyIndex = NULL_VALUE;

	GPUCommandQueue m_deferredRenderingCommandQueue = GPU_NULL_HANDLE;
	GPUCommandPool m_deferredRenderingCommandPool = GPU_NULL_HANDLE;
	GPUCommandSet m_deferredRenderingCommandSet = GPU_NULL_HANDLE;
	
	GPUCommandQueue m_shadowMappingCommandQueues[CASCADED_SHADOW_MAP_COUNT] = { GPU_NULL_HANDLE };
	GPUCommandPool m_shadowMappingCommandPools[CASCADED_SHADOW_MAP_COUNT] = { GPU_NULL_HANDLE };
	GPUCommandSet m_shadowMappingCommandSets[CASCADED_SHADOW_MAP_COUNT] = { GPU_NULL_HANDLE };
	
	GPUCommandQueue m_lightViewingCommandQueue = GPU_NULL_HANDLE;
	GPUCommandPool m_lightViewingCommandPool = GPU_NULL_HANDLE;
	GPUCommandSet m_lightViewingCommandSet = GPU_NULL_HANDLE;
	
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
	
	GPUTextureView m_shadowMappingViews[CASCADED_SHADOW_MAP_COUNT][MAX_LIGHT_COUNT] = { { GPU_NULL_HANDLE } };
	GPUFramebuffer m_shadowMappingFramebuffers[CASCADED_SHADOW_MAP_COUNT][MAX_LIGHT_COUNT] = { { GPU_NULL_HANDLE } };
	
	GPUTextureView m_geometryViews[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT] = { GPU_NULL_HANDLE };
	GPUFramebuffer m_geometryFramebuffer = GPU_NULL_HANDLE;

	GPUFramebuffer* m_lightingFramebuffers = nullptr;
	
	uint32_t m_meshCount = 0;
	GPUDescriptorPool m_graphicsBindingPool = GPU_NULL_HANDLE;

	GPUDescriptorLayout m_geometryDescriptorLayout = GPU_NULL_HANDLE;
	GPUDescriptorSet m_geometryDescriptorSet = GPU_NULL_HANDLE;

	GPUDescriptorLayout m_lightingDescriptorLayout = GPU_NULL_HANDLE;
	GPUDescriptorSet m_lightingDescriptorSet = GPU_NULL_HANDLE;
	
	GPUComputeLayout m_lightViewingLayout = GPU_NULL_HANDLE;
	GPUComputePipeline m_lightViewingPipeline = GPU_NULL_HANDLE;
	
	GPUGraphicsLayout m_shadowMappingLayout = GPU_NULL_HANDLE;
	GPUGraphicsPipeline m_shadowMappingPipelines[CASCADED_SHADOW_MAP_COUNT] = { GPU_NULL_HANDLE };

	GPUGraphicsLayout m_geometryLayout = GPU_NULL_HANDLE;
	GPUGraphicsPipeline m_geometryPipeline = GPU_NULL_HANDLE;

	GPUGraphicsLayout m_lightingLayout = GPU_NULL_HANDLE;
	GPUGraphicsPipeline m_lightingPipeline = GPU_NULL_HANDLE;
	
	GPUBuffer m_vertexBuffer = GPU_NULL_HANDLE;
	GPUIndirectDrawCommand* m_indirectCommands = nullptr;
	GPUBuffer m_indirectCommandBuffer = GPU_NULL_HANDLE;
	GPUBuffer m_instanceBuffer = GPU_NULL_HANDLE;
	GPUTextureView* m_meshTextureViews[GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT] = { nullptr };
	
	uint32_t m_lightCount = 0;
	GPUSharedAllocation m_lightAllocation = { GPU_NULL_HANDLE };
	
	GPUSampler m_sampler = GPU_NULL_HANDLE;
	
	float3 m_subFrusta[CASCADED_SHADOW_MAP_COUNT][CORNER_COUNT] = { { 0 } };
	
	Semaphore m_subFrustaTransformFinishedSemaphores[CASCADED_SHADOW_MAP_COUNT] = { NULL };
	GPUSemaphore m_imageAvailableSemaphore = GPU_NULL_HANDLE;
	GPUSemaphore m_lightViewingsFinishedSemaphore = GPU_NULL_HANDLE;
	GPUSemaphore m_shadowMappingsFinishedSemaphores[CASCADED_SHADOW_MAP_COUNT] = { VK_NULL_HANDLE };
	GPUSemaphore m_renderFinishedSemaphore = GPU_NULL_HANDLE;
	
	Semaphore m_cameraSemaphore = NULL;
	Semaphore m_instancesSemaphore = NULL;
	Semaphore m_lightsSemaphore = NULL;
	
	View m_cameraView = {
		.instance.position = { 0 },
		.instance.rotation = { 0 },
		.projection = { 0 }
	};
	
	void set_vertices(GPUStageAllocation* in_allocation, uint32_t in_meshCount, const char** in_positionFiles, const char** in_normalFiles, const char** in_uvFiles, const char** in_indexFiles, uint32_t* in_vertexCounts);
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
	
	void calculate_subFrustum(float3* in_corners, View* in_view, uint32_t in_multiplier);
	
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
	
	void build_lightViewingPipeline(void);
	void ruin_lightViewingPipeline(void);
	
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
	
	void build_semaphores(void);
	void ruin_semaphores(void);
	
	void add_instance(uint32_t in_type, Instance* in_instance);
	void rid_instance(uint32_t in_type, uint32_t in_index);
	
	void add_light(Light* in_light);
	void rid_light(uint32_t in_index);
};



#endif
