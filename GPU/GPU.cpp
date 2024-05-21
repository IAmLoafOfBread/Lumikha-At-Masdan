#include "GPU.hpp"



void* GPUFixedContext::m_shadowMappingCallback(void* in_info) {
	auto Info = static_cast<ShadowMappingInfo*>(in_info);
	Info->context->run_shadowMappings(Info->index, Info->divisor);
	return nullptr;
};

GPUFixedContext::GPUFixedContext(GLFWwindow* in_surfaceWindow, GPUExtent3D in_surfaceExtent, const uint32_t in_meshCount, const uint32_t* in_meshInstanceMaxCounts, const char** in_positionFiles, const char** in_normalFiles, const char** in_uvFiles, const char** in_indexFiles, const char*** in_textureFiles, void(*in_startupCallback)(void*)) {
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
	build_shadowMappingPipelines();
	build_geometryPipeline();
	build_lightingPipeline();
	{
		GPUStageAllocation VertexAllocations = { nullptr };
		auto VertexCounts = new uint32_t[in_meshCount];
		GPUStageAllocation* TextureAllocations[GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT] = { nullptr };
		GPUExtent3D* TextureExtents[GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT] = { nullptr };
		
		set_meshes(&VertexAllocations, in_meshCount, in_positionFiles, in_normalFiles, in_uvFiles, in_indexFiles, VertexCounts);
		for(uint32_t i = 0; i < GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT; i++) {
			TextureAllocations[i] = new GPUStageAllocation[in_meshCount];
			TextureExtents[i] = new GPUExtent3D[in_meshCount];
			for(uint32_t j = 0; j < in_meshCount; i++) {
				set_texture(&TextureAllocations[i][j], in_textureFiles[i][j], &TextureExtents[i][j]);
			}
		}
		
		build_meshes(VertexCounts, &VertexAllocations, in_meshInstanceMaxCounts, TextureAllocations, TextureExtents);
		
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
	
	Thread ShadowMappingThreads[CASCADED_SHADOW_MAP_COUNT] = { NULL };
	ShadowMappingInfo Infos[CASCADED_SHADOW_MAP_COUNT] = { NULL };
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		Infos[i].context = this;
		Infos[i].index = i;
		Infos[i].divisor = i + 1;
		create_thread(&ShadowMappingThreads[i], &m_shadowMappingCallback, static_cast<void*>(&Infos[i]));
	}
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) await_thread(ShadowMappingThreads[i]);

	run_deferredRenderings();
}

GPUFixedContext::~GPUFixedContext() {
	unset_geometryBindings();
	unset_lightingBindings();
	ruin_sampler();
	ruin_meshes();
	ruin_lights();
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



void GPUFixedContext::set_meshes(GPUStageAllocation* in_allocation, uint32_t in_meshCount, const char** in_positionFiles, const char** in_normalFiles, const char** in_uvFiles, const char** in_indexFiles, uint32_t* in_vertexCounts) {
	uint32_t TotalIndexCount = 0;
	for(uint32_t i = 0; i < in_meshCount; i++) {
		File File = open_file(in_indexFiles[i]);
		const size_t Size = get_fileSize(File);
		FileMap Map = map_file(File);
		void* View = view_fileMap(Map, Size);
		
		TotalIndexCount += static_cast<uint32_t*>(View)[0];
		
		unview_fileMap(View, Size);
		unmap_file(Map);
		close_file(File);
	}
	build_stageAllocation(in_allocation, TotalIndexCount * sizeof(Vertex));
	
	uint32_t VertexOffset = 0;
	for(uint32_t i = 0; i < in_meshCount; i++) {
		uint32_t IndexCount = 0;
		
		File PositionFile = open_file(in_positionFiles[i]);
		File NormalFile = open_file(in_normalFiles[i]);
		File UVFile = open_file(in_uvFiles[i]);
		File IndexFile = open_file(in_indexFiles[i]);
		const size_t PositionSize = get_fileSize(PositionFile);
		const size_t NormalSize = get_fileSize(NormalFile);
		const size_t UVSize = get_fileSize(UVFile);
		const size_t IndexSize = get_fileSize(IndexFile);
		FileMap PositionMap = map_file(PositionFile);
		FileMap NormalMap = map_file(NormalFile);
		FileMap UVMap = map_file(UVFile);
		FileMap IndexMap = map_file(IndexFile);
		void* PositionView = view_fileMap(PositionMap, PositionSize);
		void* NormalView = view_fileMap(NormalMap, NormalSize);
		void* UVView = view_fileMap(UVMap, UVSize);
		void* IndexView = view_fileMap(IndexMap, IndexSize);
		
		IndexCount = static_cast<uint32_t*>(IndexView)[0];
		
		auto PositionBuffer = reinterpret_cast<float3*>(&static_cast<uint32_t*>(PositionView)[1]);
		auto NormalBuffer = reinterpret_cast<float3*>(&static_cast<uint32_t*>(NormalView)[1]);
		auto UVBuffer = reinterpret_cast<float2*>(&static_cast<uint32_t*>(UVView)[1]);
		auto IndexBuffer = &static_cast<uint32_t*>(IndexView)[1];
		
		{
			uint32_t VertexIndex = 0;
			auto VertexBuffer = &static_cast<Vertex*>(in_allocation->data)[VertexOffset];
			for(uint32_t j = 0; j < IndexCount; j++) {
				VertexBuffer[j + VertexOffset].position = PositionBuffer[IndexBuffer[VertexIndex]];
				VertexIndex++;
				VertexBuffer[j + VertexOffset].normal = NormalBuffer[IndexBuffer[VertexIndex]];
				VertexIndex++;
				VertexBuffer[j + VertexOffset].uv = UVBuffer[IndexBuffer[VertexIndex]];
				VertexIndex++;
			}
			uint32_t VertexCount = IndexCount / 3;
			VertexOffset += VertexCount;
			in_vertexCounts[i] = VertexCount;
		}
		
		unview_fileMap(PositionView, PositionSize);
		unview_fileMap(NormalView, NormalSize);
		unview_fileMap(UVView, UVSize);
		unview_fileMap(IndexView, IndexSize);
		unmap_file(PositionMap);
		unmap_file(NormalMap);
		unmap_file(UVMap);
		unmap_file(IndexMap);
		close_file(PositionFile);
		close_file(NormalFile);
		close_file(UVFile);
		close_file(IndexFile);
	}
}



void GPUFixedContext::set_texture(GPUStageAllocation* in_allocation, const char* in_file, GPUExtent3D* in_extent) {
	File File = open_file(in_file);
	const size_t FileSize = get_fileSize(File);
	FileMap Map = map_file(File);
	void* View = view_fileMap(Map, FileSize);
	
	spng_ctx* CTX = spng_ctx_new(0);
	CHECK(spng_set_png_buffer(CTX, View, FileSize))
	{
		struct spng_ihdr IHDR = {};
		CHECK(spng_get_ihdr(CTX, &IHDR))
		in_extent->width = IHDR.width;
		in_extent->height = IHDR.height;
		in_extent->depth = 1;
	}
	size_t DataSize = 0;
	CHECK(spng_decoded_image_size(CTX, SPNG_FMT_PNG, &DataSize))
	build_stageAllocation(in_allocation, DataSize);
	CHECK(spng_decode_image(CTX, in_allocation->data, DataSize, SPNG_FMT_PNG, 0))
	spng_ctx_free(CTX);
	
	unview_fileMap(View, FileSize);
	unmap_file(Map);
	close_file(File);
}
