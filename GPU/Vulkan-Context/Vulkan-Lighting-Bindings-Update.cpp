#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



#define BINDING_COUNT (GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT + CASCADED_SHADOW_MAP_COUNT + 1)



void GPUFixedContext::set_lightingBindings(void) {
	VkDescriptorImageInfo GeometryInfos[GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT] = { { VK_NULL_HANDLE } };
	for (uint32_t i = 0; i < GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT; i++) {
		GeometryInfos[i].sampler = m_sampler;
		GeometryInfos[i].imageView = m_geometryTextures[i].view;
		GeometryInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}
	VkDescriptorImageInfo ShadowInfos[CASCADED_SHADOW_MAP_COUNT][MAX_LIGHT_COUNT] = { { { VK_NULL_HANDLE } } };
	for (uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		for (uint32_t j = 0; j < MAX_LIGHT_COUNT; j++) {
			ShadowInfos[i][j].sampler = m_sampler;
			ShadowInfos[i][j].imageView = m_shadowTextures[i][j].view;
			ShadowInfos[i][j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
	}
	const VkDescriptorImageInfo ReflectionInfo = {
		.sampler = m_sampler,
		.imageView = m_reflectionTexture.view,
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	};
	

	VkWriteDescriptorSet Writes[BINDING_COUNT] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	for (uint32_t i = 0; i < BINDING_COUNT; i++) {
		Writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		Writes[i].pNext = nullptr;
		Writes[i].dstSet = m_lightingDescriptorSet;
		Writes[i].dstBinding = i;
		Writes[i].dstArrayElement = 0;
		Writes[i].descriptorCount = 1;
		Writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		Writes[i].pImageInfo = nullptr;
		Writes[i].pBufferInfo = nullptr;
		Writes[i].pTexelBufferView = nullptr;
	}
	for (uint32_t i = 0; i < GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT; i++) {
		Writes[i].pImageInfo = &GeometryInfos[i];
	}
	for (uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		Writes[i + GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT].descriptorCount = MAX_LIGHT_COUNT;
		Writes[i + GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT].pImageInfo = ShadowInfos[i];
	}
	Writes[BINDING_COUNT - 1].pImageInfo = &ReflectionInfo;

	vkUpdateDescriptorSets(m_logical, BINDING_COUNT, Writes, 0, nullptr);
}



#endif
