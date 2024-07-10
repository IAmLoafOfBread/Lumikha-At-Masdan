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


#define MAX_STRING_LENGTH 255



class GPUFixedContext {
public:
	GPUFixedContext(uint32_t in_directoryLength, char* in_directory, GLFWwindow* in_surfaceWindow, GPUExtent3D in_surfaceExtent, const uint32_t in_meshCount, const uint32_t* in_instanceMaxCounts, const char** in_positionFiles, const char** in_normalFiles, const char** in_uvFiles, const char** in_indexFiles, const char*** in_textureFiles);
	~GPUFixedContext();

	uint32_t m_directoryLength;
	char m_directory[MAX_STRING_LENGTH];

	bool m_multiThreadedGraphics;
	
	Instance* m_instances;
	Light* m_lights;
	
	Camera m_cameraData;
	
	void initialize_imageAcquiring(void);
	void acquire_nextImage(void);

	void calculate_subFrustum(uint32_t in_index, uint32_t in_multiplier);
	
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
	const GPUSize m_fixedOffset;
	uint32_t m_currentImageIndex;
	
	GLFWwindow* m_surfaceWindow;
	
	uint32_t m_localMemoryIndex;
	uint32_t m_sharedMemoryIndex;
	GPUDevice m_logical;
	
	uint32_t m_queueFamilyCount;
	uint32_t m_graphicsQueueFamilyIndex;
	uint32_t m_computeQueueFamilyIndex;

	GPUCommandQueue m_deferredRenderingCommandQueue;
	GPUCommandPool m_deferredRenderingCommandPool;
	GPUCommandSet m_deferredRenderingCommandSet;
	
	GPUCommandQueue m_shadowMappingCommandQueues[CASCADED_SHADOW_MAP_COUNT];
	GPUCommandPool m_shadowMappingCommandPools[CASCADED_SHADOW_MAP_COUNT];
	GPUCommandSet m_shadowMappingCommandSets[CASCADED_SHADOW_MAP_COUNT];
	
	GPUCommandQueue m_lightViewingCommandQueue;
	GPUCommandPool m_lightViewingCommandPool;
	GPUCommandSet m_lightViewingCommandSet;
	
	GPUSurface m_surface;
	uint32_t m_surfaceFrameCount;
	GPUExtent3D m_surfaceExtent;
	GPUFormat m_surfaceFormat;
	GPUColourSpace m_surfaceColourSpace;
	
	GPURenderPass m_shadowMappingPass;
	
	GPURenderPass m_geometryPass;
	
	GPURenderPass m_lightingPass;
	
	GPUSwapchain m_swapchain;
	GPUTextureImage* m_presentImages;
	GPUTextureView* m_presentViews;
	
	GPULocalTexture m_shadowTextures[CASCADED_SHADOW_MAP_COUNT][MAX_LIGHT_COUNT];
	GPUFramebuffer m_shadowMappingFramebuffers[CASCADED_SHADOW_MAP_COUNT][MAX_LIGHT_COUNT];
	
	GPULocalTexture m_geometryTextures[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT + 1];
	GPUFramebuffer m_geometryFramebuffer;

	GPUFramebuffer* m_lightingFramebuffers;
	
	uint32_t m_meshCount;
	GPUDescriptorPool m_graphicsBindingPool;

	GPUDescriptorLayout m_geometryDescriptorLayout;
	GPUDescriptorSet m_geometryDescriptorSet;

	GPUDescriptorLayout m_lightingDescriptorLayout;
	GPUDescriptorSet m_lightingDescriptorSet;
	
	GPUComputeLayout m_lightViewingLayout;
	GPUComputePipeline m_lightViewingPipeline;
	
	GPUGraphicsLayout m_shadowMappingLayout;
	GPUGraphicsPipeline m_shadowMappingPipelines[CASCADED_SHADOW_MAP_COUNT];

	GPUGraphicsLayout m_geometryLayout;
	GPUGraphicsPipeline m_geometryPipeline;

	GPUGraphicsLayout m_lightingLayout;
	GPUGraphicsPipeline m_lightingPipeline;
	
	GPUBuffer m_vertexBuffer;
	GPUIndirectDrawCommand* m_indirectCommands;
	GPUBuffer m_indirectCommandBuffer;
	GPUBuffer m_instanceBuffer;
	GPUTextureView* m_meshTextureViews[GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT];
	
	uint32_t m_lightCount;
	GPUSharedAllocation m_lightAllocation;
	
	GPUSampler m_sampler;
	
	GPUSharedAllocation m_subFrustumAllocation;
	float3* m_subFrusta;
	
	uint64_t m_imageAvailableStatus;
	GPUSemaphore m_imageAvailableSemaphore;

	uint64_t m_lightViewingsFinishedStatus;
	GPUSemaphore m_lightViewingsFinishedSemaphore;

	GPUSemaphore m_shadowMappingsFinishedSemaphores[CASCADED_SHADOW_MAP_COUNT];

	GPUSemaphore m_renderFinishedSemaphore;
	
	Semaphore m_cameraSemaphore;
	Semaphore m_instancesSemaphore;
	Semaphore m_lightsSemaphore;
	
	View m_cameraView;
	
	void set_vertices(GPUStageAllocation* in_allocation, const char** in_positionPaths, const char** in_normalPaths, const char** in_uvPaths, const char** in_indexPaths, uint32_t* in_vertexCounts);
	void set_texture(GPUStageAllocation* in_allocation, const char* in_path, GPUExtent3D* in_extent);
	
	void build_stageAllocation(GPUStageAllocation* in_stage, GPUSize in_size);
	void build_sharedAllocation(GPUSharedAllocation* in_shared, GPUSize in_size, GPUBufferUsageFlags in_flags, bool in_computeIndexInclude);
	void ruin_sharedAllocation(GPUSharedAllocation* in_shared);
	void build_localAllocation(GPULocalAllocation* in_local, GPUStageAllocation* in_stage, GPUBufferUsageFlags in_flags);
	void ruin_localAllocation(GPULocalAllocation* in_local);
	void build_localTexture(GPULocalTexture* in_texture, GPUStageAllocation* in_stage, GPUFormat in_format, GPUExtent3D in_extent, GPUImageUsageFlags in_flags, GPUImageLayout in_layout);
	void ruin_localTexture(GPULocalTexture* in_texture);
	
	void build_module(GPUModule* in_module, const char* in_path);
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

	void build_subFrusta(void);
	void ruin_subFrusta(void);

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
