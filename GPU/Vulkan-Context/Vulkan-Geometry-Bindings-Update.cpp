#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::set_geometryBindings(void) {
	VkDescriptorImageInfo* Infos[GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT] = { { VK_NULL_HANDLE } };
	VkWriteDescriptorSet Writes[GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT] = { };
	for(uint32_t i = 0; i < GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT; i++) {
		Infos[i] = new VkDescriptorImageInfo[m_meshCount];
		for (uint32_t j = 0; j < m_meshCount; j++) {
			Infos[i][j].sampler = m_sampler;
			Infos[i][j].imageView = m_meshTextureViews[i][j];
			Infos[i][j].imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
		}

		Writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		Writes[i].pNext = nullptr;
		Writes[i].dstSet = m_geometryDescriptorSet;
		Writes[i].dstBinding = i;
		Writes[i].dstArrayElement = 0;
		Writes[i].descriptorCount = m_meshCount;
		Writes[i].pImageInfo = Infos[i];
		Writes[i].pBufferInfo = nullptr;
		Writes[i].pTexelBufferView = nullptr;
	}
	vkUpdateDescriptorSets(m_logical, GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT, Writes, 0, nullptr);

	for (uint32_t i = 0; i < GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT; i++) {
		delete[] Infos[i];
	}
}



#endif
