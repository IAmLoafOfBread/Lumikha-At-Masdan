#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static GPULocalAllocation g_vertexAllocation = { VK_NULL_HANDLE };
static GPUSharedAllocation g_indirectCommandAllocation = { VK_NULL_HANDLE };
static GPUSharedAllocation g_instanceAllocation = { VK_NULL_HANDLE };
static GPULocalTexture* g_textures[GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT] = { VK_NULL_HANDLE };



void GPUFixedContext::build_meshes(uint32_t* in_vertexCounts, GPUStageAllocation* in_vertexAllocation, const uint32_t* in_maxInstanceCounts, GPUStageAllocation** in_textureAllocations, GPUExtent3D** in_extents) {
	build_localAllocation(&g_vertexAllocation, in_vertexAllocation, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	m_vertexBuffer = g_vertexAllocation.buffer;
	
	build_sharedAllocation(&g_indirectCommandAllocation, m_meshCount * sizeof(*m_indirectCommands), VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT, false);
	m_indirectCommands = static_cast<GPUIndirectDrawCommand*>(g_indirectCommandAllocation.data);
	m_indirectCommandBuffer = g_indirectCommandAllocation.buffer;
	
	uint32_t VertexCount = 0;
	uint32_t InstanceCount = 0;
	for(uint32_t i = 0; i < m_meshCount; i++) {
		m_indirectCommands[i].vertexCount = in_vertexCounts[i];
		m_indirectCommands[i].instanceCount = 0;
		m_indirectCommands[i].firstVertex = VertexCount;
		m_indirectCommands[i].firstInstance = InstanceCount;
		VertexCount += in_vertexCounts[i];
		InstanceCount += in_maxInstanceCounts[i];
	}
	
	build_sharedAllocation(&g_instanceAllocation, InstanceCount * sizeof(Instance), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, false);
	m_instanceBuffer = g_instanceAllocation.buffer;
	m_instances = static_cast<Instance*>(g_instanceAllocation.data);
	
	const VkFormat Formats[GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT] = { GEOMETRY_PASS_REQUIRED_TEXTURE_FORMATS };
	for(uint32_t i = 0; i < GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT; i++) {
		g_textures[i] = new GPULocalTexture[m_meshCount];
		m_meshTextureViews[i] = new GPUTextureView[m_meshCount];
		for(uint32_t j = 0; j < m_meshCount; j++) {
			build_localTexture(&g_textures[i][j], &in_textureAllocations[i][j], Formats[i], in_extents[i][j], VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);
			m_meshTextureViews[i][j] = g_textures[i][j].view;
		}
	}
	
}

void GPUFixedContext::ruin_meshes(void) {
	for(uint32_t i = 0; i < GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT; i++) {
		for(uint32_t j = 0; j < m_meshCount; j++) {
			ruin_localTexture(&g_textures[i][j]);
		}
		delete[] g_textures[i];
		delete[] m_meshTextureViews[i];
	}
	ruin_sharedAllocation(&g_instanceAllocation);
	ruin_sharedAllocation(&g_indirectCommandAllocation);
	ruin_localAllocation(&g_vertexAllocation);
}



#endif
