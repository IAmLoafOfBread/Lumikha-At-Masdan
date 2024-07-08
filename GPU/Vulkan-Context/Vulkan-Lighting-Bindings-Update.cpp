#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



#define BINDING_COUNT (GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT + CASCADED_SHADOW_MAP_COUNT + 1)
#define BUFFER_INDEX (BINDING_COUNT - 1)



void GPUFixedContext::set_lightingBindings(void) {
	VkDescriptorImageInfo InputInfos[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT] = { 0 };
	for (uint32_t i = 0; i < GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT; i++) {
		InputInfos[i].sampler = m_sampler;
		InputInfos[i].imageView = m_geometryTextures[i].view;
		InputInfos[i].imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
	}
	VkDescriptorImageInfo* ShadowInfos[CASCADED_SHADOW_MAP_COUNT] = { nullptr };
	for (uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		ShadowInfos[i] = new VkDescriptorImageInfo[MAX_LIGHT_COUNT];
		for (uint32_t j = 0; j < MAX_LIGHT_COUNT; j++) {
			ShadowInfos[i][j].sampler = m_sampler;
			ShadowInfos[i][j].imageView = m_shadowTextures[i][j].view;
			ShadowInfos[i][j].imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
		}
	}
	const VkDescriptorBufferInfo BufferInfo = {
		.buffer = m_lightAllocation.buffer,
		.offset = 0,
		.range = VK_WHOLE_SIZE
	};

	VkWriteDescriptorSet Writes[BINDING_COUNT] = { };
	for (uint32_t i = 0; i < BINDING_COUNT; i++) {
		Writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		Writes[i].pNext = nullptr;
		Writes[i].dstSet = m_geometryDescriptorSet;
		Writes[i].dstBinding = i;
		Writes[i].dstArrayElement = 0;
		Writes[i].descriptorCount = 1;
		Writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		Writes[i].pImageInfo = nullptr;
		Writes[i].pBufferInfo = nullptr;
		Writes[i].pTexelBufferView = nullptr;
	}
	for (uint32_t i = 0; i < GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT; i++) Writes[i].pImageInfo = &InputInfos[i];
	for (uint32_t i = GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT; i < BUFFER_INDEX; i++) {
		Writes[i].descriptorCount = MAX_LIGHT_COUNT;
		Writes[i].pImageInfo = ShadowInfos[i];
	}
	Writes[BUFFER_INDEX].descriptorCount = 1;
	Writes[BUFFER_INDEX].pBufferInfo = &BufferInfo;
	vkUpdateDescriptorSets(m_logical, GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT, Writes, 0, nullptr);

	for (uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		delete[] ShadowInfos[i];
	}
}



#endif
