#include "GPU.hpp"



GPUFixedContext::GPUFixedContext(GLFWwindow* in_surfaceWindow, GPUExtent3D in_surfaceExtent, const uint32_t in_meshCount, const uint32_t* in_instanceMaxCounts, const char** in_positionFiles, const char** in_normalFiles, const char** in_uvFiles, const char** in_indexFiles, const char*** in_textureFiles) :
	m_multiThreadedGraphics(false),
	m_instances(nullptr),
	m_lights(nullptr),
	m_fixedOffset(0),
	m_currentImageIndex(0),
	m_surfaceWindow(nullptr),
	m_localMemoryIndex(0),
	m_sharedMemoryIndex(0),
	m_logical(GPU_NULL_HANDLE),
	m_queueFamilyCount(0),
	m_graphicsQueueFamilyIndex(NULL_VALUE),
	m_computeQueueFamilyIndex(NULL_VALUE),
	m_deferredRenderingCommandQueue(GPU_NULL_HANDLE),
	m_deferredRenderingCommandPool(GPU_NULL_HANDLE),
	m_deferredRenderingCommandSet(GPU_NULL_HANDLE),
	m_shadowMappingCommandQueues{ GPU_NULL_HANDLE },
	m_shadowMappingCommandPools{ GPU_NULL_HANDLE },
	m_shadowMappingCommandSets{ GPU_NULL_HANDLE },
	m_lightViewingCommandQueue(GPU_NULL_HANDLE),
	m_lightViewingCommandPool(GPU_NULL_HANDLE),
	m_lightViewingCommandSet(GPU_NULL_HANDLE),
	m_surface(GPU_NULL_HANDLE),
	m_surfaceFrameCount(0),
	m_surfaceExtent{ 0 },
	m_surfaceFormat(static_cast<GPUFormat>(0)),
	m_surfaceColourSpace(static_cast<GPUColourSpace>(0)),
	m_shadowMappingPass(GPU_NULL_HANDLE),
	m_geometryPass(GPU_NULL_HANDLE),
	m_lightingPass(GPU_NULL_HANDLE),
	m_swapchain(GPU_NULL_HANDLE),
	m_presentViews(nullptr),
	m_shadowMappingViews{ { GPU_NULL_HANDLE } },
	m_shadowMappingFramebuffers{ { GPU_NULL_HANDLE } },
	m_geometryViews{ GPU_NULL_HANDLE },
	m_geometryFramebuffer(GPU_NULL_HANDLE),
	m_lightingFramebuffers(nullptr),
	m_meshCount(0),
	m_graphicsBindingPool(GPU_NULL_HANDLE),
	m_geometryDescriptorLayout(GPU_NULL_HANDLE),
	m_geometryDescriptorSet(GPU_NULL_HANDLE),
	m_lightingDescriptorLayout(GPU_NULL_HANDLE),
	m_lightingDescriptorSet(GPU_NULL_HANDLE),
	m_lightViewingLayout(GPU_NULL_HANDLE),
	m_lightViewingPipeline(GPU_NULL_HANDLE),
	m_shadowMappingLayout(GPU_NULL_HANDLE),
	m_shadowMappingPipelines{ GPU_NULL_HANDLE },
	m_geometryLayout(GPU_NULL_HANDLE),
	m_geometryPipeline(GPU_NULL_HANDLE),
	m_lightingLayout(GPU_NULL_HANDLE),
	m_lightingPipeline(GPU_NULL_HANDLE),
	m_vertexBuffer(GPU_NULL_HANDLE),
	m_indirectCommands(nullptr),
	m_indirectCommandBuffer(GPU_NULL_HANDLE),
	m_instanceBuffer(GPU_NULL_HANDLE),
	m_meshTextureViews{ nullptr },
	m_lightCount(0),
	m_lightAllocation{ GPU_NULL_HANDLE },
	m_sampler(GPU_NULL_HANDLE),
	m_subFrusta{ { 0.0f } },
	m_subFrustaTransformFinishedSemaphores{ NULL },
	m_imageAvailableSemaphore(GPU_NULL_HANDLE),
	m_lightViewingsFinishedSemaphore(GPU_NULL_HANDLE),
	m_shadowMappingsFinishedSemaphores{ VK_NULL_HANDLE },
	m_renderFinishedSemaphore(GPU_NULL_HANDLE),
	m_cameraSemaphore(NULL),
	m_instancesSemaphore(NULL),
	m_lightsSemaphore(NULL),
	m_cameraView{ 0.0f } {
	m_cameraView.projection.vecs[2].w = 1.0f;
	
	m_surfaceWindow = in_surfaceWindow;
	build_device(in_surfaceWindow, in_surfaceExtent);
	build_shadowMappingPass();
	build_geometryPass();
	build_lightingPass();
	build_swapchain();
	build_shadowMappingFramebuffers();
	build_geometryFramebuffer();
	build_lightingFramebuffers();
	build_graphicsBindingPool(in_meshCount);
	build_geometryBindings();
	build_lightingBindings();
	build_lightViewingPipeline();
	build_shadowMappingPipelines();
	build_geometryPipeline();
	build_lightingPipeline();
	{
		GPUStageAllocation VertexAllocations = { nullptr };
		auto VertexCounts = new uint32_t[in_meshCount];
		GPUStageAllocation* TextureAllocations[GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT] = { nullptr };
		GPUExtent3D* TextureExtents[GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT] = { nullptr };
		
		set_vertices(&VertexAllocations, in_meshCount, in_positionFiles, in_normalFiles, in_uvFiles, in_indexFiles, VertexCounts);
		for(uint32_t i = 0; i < GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT; i++) {
			TextureAllocations[i] = new GPUStageAllocation[in_meshCount];
			TextureExtents[i] = new GPUExtent3D[in_meshCount];
			for(uint32_t j = 0; j < in_meshCount; i++) {
				set_texture(&TextureAllocations[i][j], in_textureFiles[i][j], &TextureExtents[i][j]);
			}
		}
		
		build_meshes(VertexCounts, &VertexAllocations, in_instanceMaxCounts, TextureAllocations, TextureExtents);
		
		delete[] VertexCounts;
		for(uint32_t i = 0; i < GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT; i++) {
			delete[] TextureAllocations[i];
			delete[] TextureExtents[i];
		}
	}
	build_lights();
	build_sampler();
	
	set_geometryBindings();
	set_lightingBindings();
	
	build_semaphores();
	
	update_camera();
}

GPUFixedContext::~GPUFixedContext() {
	ruin_semaphores();
	ruin_sampler();
	ruin_meshes();
	ruin_lights();
	ruin_lightViewingPipeline();
	ruin_shadowMappingPipelines();
	ruin_geometryPipeline();
	ruin_lightingPipeline();
	ruin_geometryBindings();
	ruin_lightingBindings();
	ruin_graphicsBindingPool();
	ruin_shadowMappingFramebuffers();
	ruin_geometryFramebuffer();
	ruin_lightingFramebuffers();
	ruin_swapchain();
	ruin_shadowMappingPass();
	ruin_geometryPass();
	ruin_lightingPass();
	ruin_device();

}
