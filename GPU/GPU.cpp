#include "GPU.hpp"



#define name(v) "/ LaM::"#v".sem"



GPUFixedContext::GPUFixedContext(uint32_t in_directoryLength, char* in_directory, GLFWwindow* in_surfaceWindow, const uint32_t in_meshCount, const uint32_t* in_instanceMaxCounts, const char** in_positionPaths, const char** in_normalPaths, const char** in_uvPaths, const char** in_indexPaths, const char*** in_texturePaths) :
	m_directoryLength(in_directoryLength),
	m_directory{ 0 },
	m_multiThreadedGraphics(false),
	m_instances(nullptr),
	m_lights(nullptr),
	m_cameraData{1.5f, 100.0f, 0.1f},
	m_fixedOffset(0),
	m_currentImageIndex(NULL_VALUE),
	m_surfaceWindow(in_surfaceWindow),
	m_localMemoryIndex(NULL_VALUE),
	m_sharedMemoryIndex(NULL_VALUE),
	m_queueFamilyCount(NULL_VALUE),
	m_graphicsQueueFamilyIndex(NULL_VALUE),
	m_computeQueueFamilyIndex(NULL_VALUE),
	m_computeQueueIndex(0),
	m_context(GPU_NULL_HANDLE),
	m_physical(GPU_NULL_HANDLE),
	m_logical(GPU_NULL_HANDLE),
	m_lightViewingCommandQueue(GPU_NULL_HANDLE),
	m_lightViewingCommandPool(GPU_NULL_HANDLE),
	m_lightViewingCommandSet(GPU_NULL_HANDLE),
	m_shadowMappingCommandQueues{ GPU_NULL_HANDLE },
	m_shadowMappingCommandPools{ GPU_NULL_HANDLE },
	m_shadowMappingCommandSets{ GPU_NULL_HANDLE },
	m_deferredRenderingCommandQueue(GPU_NULL_HANDLE),
	m_geometryCommandPool(GPU_NULL_HANDLE),
	m_geometryCommandSet(GPU_NULL_HANDLE),
	m_lightingCommandPool(GPU_NULL_HANDLE),
	m_lightingCommandSet(GPU_NULL_HANDLE),
	m_presentCommandPool(GPU_NULL_HANDLE),
	m_presentCommandSet(GPU_NULL_HANDLE),
	m_surface(GPU_NULL_HANDLE),
	m_surfaceFrameCount(0),
	m_surfaceExtent{0, 0, 1},
	m_surfaceFormat(static_cast<GPUFormat>(0)),
	m_surfaceColourSpace(static_cast<GPUColourSpace>(0)),
	m_shadowMappingPass(GPU_NULL_HANDLE),
	m_geometryPass(GPU_NULL_HANDLE),
	m_lightingPass(GPU_NULL_HANDLE),
	m_postProcessingPass(GPU_NULL_HANDLE),
	m_swapchain(GPU_NULL_HANDLE),
	m_presentImages(nullptr),
	m_presentViews(nullptr),
	m_shadowTextures{ { GPU_NULL_HANDLE } },
	m_shadowMappingFramebuffers{ { GPU_NULL_HANDLE } },
	m_geometryTextures{ GPU_NULL_HANDLE },
	m_geometryFramebuffer(GPU_NULL_HANDLE),
	m_lightingFramebuffer(GPU_NULL_HANDLE),
	m_postProcessingFramebuffers(nullptr),
	m_meshCount(in_meshCount),
	m_graphicsBindingPool(GPU_NULL_HANDLE),
	m_geometryDescriptorLayout(GPU_NULL_HANDLE),
	m_geometryDescriptorSet(GPU_NULL_HANDLE),
	m_lightingDescriptorLayout(GPU_NULL_HANDLE),
	m_lightingDescriptorSet(GPU_NULL_HANDLE),
	m_postProcessingDescriptorLayout(GPU_NULL_HANDLE),
	m_postProcessingDescriptorSet(GPU_NULL_HANDLE),
	m_lightViewingLayout(GPU_NULL_HANDLE),
	m_lightViewingPipeline(GPU_NULL_HANDLE),
	m_shadowMappingLayout(GPU_NULL_HANDLE),
	m_shadowMappingPipelines{ GPU_NULL_HANDLE },
	m_geometryLayout(GPU_NULL_HANDLE),
	m_geometryPipeline(GPU_NULL_HANDLE),
	m_lightingLayout(GPU_NULL_HANDLE),
	m_lightingPipeline(GPU_NULL_HANDLE),
	m_postProcessingLayout(GPU_NULL_HANDLE),
	m_postProcessingPipeline(GPU_NULL_HANDLE),
	m_reflectionSampleAllocation{ GPU_NULL_HANDLE },
	m_occlusionSampleAllocation{ GPU_NULL_HANDLE },
	m_vertexBuffer(GPU_NULL_HANDLE),
	m_indirectCommands(nullptr),
	m_indirectCommandBuffer(GPU_NULL_HANDLE),
	m_instanceBuffer(GPU_NULL_HANDLE),
	m_meshTextureViews{ nullptr },
	m_lightCount(0),
	m_lightAllocation{ GPU_NULL_HANDLE },
	m_sampler(GPU_NULL_HANDLE),
	m_subFrustumAllocation{ GPU_NULL_HANDLE },
	m_subFrusta(nullptr),
	m_lightViewingsFinishedFence(GPU_NULL_HANDLE),
	m_shadowMappingsFinishedFences{ GPU_NULL_HANDLE },
	m_geometryFinishedFence(GPU_NULL_HANDLE),
	m_lightingFinishedFence(GPU_NULL_HANDLE),
	m_lightViewingsFinishedSemaphores{ GPU_NULL_HANDLE },
	m_shadowMappingsFinishedSemaphores{ GPU_NULL_HANDLE },
	m_geometryFinishedSemaphore(GPU_NULL_HANDLE),
	m_lightingFinishedSemaphore(GPU_NULL_HANDLE),
	m_cameraSemaphore(NULL),
	m_instancesSemaphore(NULL),
	m_lightsSemaphore(NULL),
	m_cameraView{ 0.0f } {
	m_cameraView.projection.vecs[2].w = 1.0f;

	for(uint32_t i = 0; i < in_directoryLength; i++) {
		m_directory[i] = in_directory[i];
	}

	build_context();
	obtain_physical();
	build_logical();
	build_commandThreads();
	build_surface();
	build_shadowMappingPass();
	build_geometryPass();
	build_lightingPass();
	build_postProcessingPass();
	build_swapchain();
	build_shadowMappingFramebuffers();
	build_geometryFramebuffer();
	build_lightingFramebuffer();
	build_postProcessingFramebuffers();
	build_graphicsBindingPool(in_meshCount);
	build_geometryBindings();
	build_lightingBindings();
	build_postProcessingBindings();
	build_lightViewingPipeline();
	build_shadowMappingPipelines();
	build_geometryPipeline();
	build_lightingPipeline();
	build_postProcessingPipeline();
	build_lightingSamples();
	{
		GPUStageAllocation VertexAllocations = { nullptr };
		auto VertexCounts = new uint32_t[in_meshCount];
		GPUStageAllocation* TextureAllocations[GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT] = { nullptr };
		GPUExtent3D* TextureExtents[GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT] = { nullptr };
		
		set_vertices(&VertexAllocations, in_positionPaths, in_normalPaths, in_uvPaths, in_indexPaths, VertexCounts);
		for(uint32_t i = 0; i < GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT; i++) {
			TextureAllocations[i] = new GPUStageAllocation[in_meshCount];
			TextureExtents[i] = new GPUExtent3D[in_meshCount];
			for(uint32_t j = 0; j < in_meshCount; j++) {
				set_texture(&TextureAllocations[i][j], in_texturePaths[i][j], &TextureExtents[i][j]);
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
	build_subFrusta();
	
	set_geometryBindings();
	set_lightingBindings();
	set_postProcessingBindings();
	
	build_synchronizations();

	create_semaphore(&m_cameraSemaphore, name(m_cameraSemaphore));
	create_semaphore(&m_instancesSemaphore, name(m_instancesSemaphore));
	create_semaphore(&m_lightsSemaphore, name(m_lightsSemaphore));
	
	update_camera();

	initialize_instanceUpdateData();
	initialize_lightUpdateData();

	initialize_lightViewingUpdateData();
	initialize_shadowMappingUpdateData();
	initialize_geometryUpdateData();
	initialize_lightingUpdateData();
	initialize_presentUpdateData();
}

GPUFixedContext::~GPUFixedContext() {
	await_device();
	destroy_semaphore(m_cameraSemaphore, name(m_cameraSemaphore));
	destroy_semaphore(m_instancesSemaphore, name(m_instancesSemaphore));
	destroy_semaphore(m_lightsSemaphore, name(m_lightsSemaphore));
	ruin_synchronizations();
	ruin_subFrusta();
	ruin_sampler();
	ruin_meshes();
	ruin_lights();
	ruin_lightingSamples();
	ruin_lightViewingPipeline();
	ruin_shadowMappingPipelines();
	ruin_geometryPipeline();
	ruin_lightingPipeline();
	ruin_postProcessingPipeline();
	ruin_geometryBindings();
	ruin_lightingBindings();
	ruin_postProcessingBindings();
	ruin_graphicsBindingPool();
	ruin_shadowMappingFramebuffers();
	ruin_geometryFramebuffer();
	ruin_lightingFramebuffer();
	ruin_postProcessingFramebuffers();
	ruin_swapchain();
	ruin_shadowMappingPass();
	ruin_geometryPass();
	ruin_lightingPass();
	ruin_postProcessingPass();
	ruin_surface();
	ruin_commandThreads();
	ruin_logical();
	ruin_context();
}
