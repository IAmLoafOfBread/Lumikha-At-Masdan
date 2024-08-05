#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::set_postProcessingBindings(void) {
	VkDescriptorImageInfo Infos[POST_PROCESSING_BINDING_COUNT] = {
		{
			.imageView = m_lightingTexture.view
		},
		{
			.imageView = m_geometryTextures[0].view
		},
		{
			.imageView = m_geometryTextures[1].view
		},
		{
			.imageView = m_geometryTextures[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT].view
		}
	};
	for(uint32_t i = 0; i < POST_PROCESSING_BINDING_COUNT; i++) {
		Infos[i].sampler = m_sampler;
		Infos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}
	
	VkWriteDescriptorSet Writes[POST_PROCESSING_BINDING_COUNT] = { { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET } };
	for (uint32_t i = 0; i < POST_PROCESSING_BINDING_COUNT; i++) {
		Writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		Writes[i].pNext = nullptr;
		Writes[i].dstSet = m_postProcessingDescriptorSet;
		Writes[i].dstBinding = i;
		Writes[i].dstArrayElement = 0;
		Writes[i].descriptorCount = 1;
		Writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		Writes[i].pImageInfo = &Infos[i];
		Writes[i].pBufferInfo = nullptr;
		Writes[i].pTexelBufferView = nullptr;
	}
	
	vkUpdateDescriptorSets(m_logical, POST_PROCESSING_BINDING_COUNT, Writes, 0, nullptr);
}



#endif

