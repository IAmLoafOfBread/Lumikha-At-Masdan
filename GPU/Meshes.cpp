#include "GPU.hpp"



void GPUFixedContext::set_vertices(GPUStageAllocation* in_allocation, const char** in_positionPaths, const char** in_normalPaths, const char** in_uvPaths, const char** in_indexPaths, uint32_t* in_vertexCounts) {	
	uint32_t TotalIndexCount = 0;
	for(uint32_t i = 0; i < m_meshCount; i++) {
		File File = open_file(in_indexPaths[i]);
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
	for(uint32_t i = 0; i < m_meshCount; i++) {
		uint32_t IndexCount = 0;
		
		File PositionFile = open_file(in_positionPaths[i]);
		File NormalFile = open_file(in_normalPaths[i]);
		File UVFile = open_file(in_uvPaths[i]);
		File IndexFile = open_file(in_indexPaths[i]);
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
			uint32_t VertexCount = IndexCount / 3;
			auto VertexBuffer = &static_cast<Vertex*>(in_allocation->data)[VertexOffset];
			for(uint32_t j = 0; j < VertexCount; j++) {
				VertexBuffer[j + VertexOffset].position = PositionBuffer[IndexBuffer[VertexIndex]];
				VertexIndex++;
				VertexBuffer[j + VertexOffset].normal = NormalBuffer[IndexBuffer[VertexIndex]];
				VertexIndex++;
				VertexBuffer[j + VertexOffset].uv = UVBuffer[IndexBuffer[VertexIndex]];
				VertexIndex++;
			}
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



void GPUFixedContext::set_texture(GPUStageAllocation* in_allocation, const char* in_path, GPUExtent3D* in_extent) {
	File File = open_file(in_path);
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
