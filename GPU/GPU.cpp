#include "GPU.hpp"



GPUFixedContext::GPUFixedContext(GLFWwindow* in_surfaceWindow, GPUExtent3D in_surfaceExtent, const uint32_t in_meshCount, const uint32_t* in_instanceMaxCounts, const char** in_positionFiles, const char** in_normalFiles, const char** in_uvFiles, const char** in_indexFiles, const char*** in_textureFiles) {
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
