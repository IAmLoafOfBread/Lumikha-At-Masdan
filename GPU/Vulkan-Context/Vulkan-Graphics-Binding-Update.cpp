#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkSampler g_sampler = VK_NULL_HANDLE;



void GPUFixedContext::set_deferredRenderingBindings(void) {
	{
		const VkSamplerCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.mipLodBias = 0,
			.anisotropyEnable = VK_FALSE,
			.maxAnisotropy = 0,
			.compareEnable = VK_FALSE,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.minLod = 0,
			.maxLod = 0,
			.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			.unnormalizedCoordinates = VK_FALSE
		};
		CHECK(vkCreateSampler(m_logical, &CreateInfo, nullptr, &g_sampler))
	}
	
	VkWriteDescriptorSet Writes[DEFERRED_RENDERING_BINDING_COUNT] = {  };
	
	for(uint32_t i = 0; i < DEFERRED_RENDERING_BINDING_COUNT; i++) {
		Writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		Writes[i].pNext = nullptr;
		Writes[i].dstSet = m_deferredRenderingDescriptorSet;
		Writes[i].dstBinding = i;
		Writes[i].dstArrayElement = 0;
		Writes[i].descriptorCount = 0;
		Writes[i].pImageInfo = nullptr;
		Writes[i].pBufferInfo = nullptr;
		Writes[i].pTexelBufferView = nullptr;
	}
	
	VkDescriptorImageInfo InputInfos[LIGHTING_PASS_INPUT_COUNT] = { 0 };
	uint32_t Min = 0;
	uint32_t Cap = LIGHTING_PASS_INPUT_COUNT;
	for(uint32_t i = Min; i < Cap; i++) {
		InputInfos[i].sampler = nullptr;
		InputInfos[i].imageView = m_geometryOutputAttachments[i].view;
		InputInfos[i].imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
		Writes[i].descriptorCount = 1;
		Writes[i].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		Writes[i].pImageInfo = &InputInfos[i];
	}

	Min = Cap;
	Cap += DEFERRED_RENDERING_REQUIRED_TEXTURE_COUNT;
	VkDescriptorImageInfo* TextureInfos[DEFERRED_RENDERING_REQUIRED_TEXTURE_COUNT] = { nullptr };
	for(uint32_t i = 0; i < DEFERRED_RENDERING_REQUIRED_TEXTURE_COUNT; i++) {
		TextureInfos[i] = new VkDescriptorImageInfo[m_meshCount];
		for(uint32_t j = 0; j < m_meshCount; j++) {
			TextureInfos[i][j].sampler = g_sampler;
			TextureInfos[i][j].imageView = m_graphicsMeshTextureViews[i][j];
			TextureInfos[i][j].imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
		}
		Writes[i + Min].descriptorCount = m_meshCount;
		Writes[i + Min].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		Writes[i + Min].pImageInfo = TextureInfos[i];
	}
	
	Min = Cap;
	Cap += CASCADED_SHADOW_MAP_COUNT;
	VkDescriptorImageInfo ShadowingInfos[CASCADED_SHADOW_MAP_COUNT][MAX_LIGHT_COUNT] = { { nullptr } };
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		for(uint32_t j = 0; j < MAX_LIGHT_COUNT; j++) {
			ShadowingInfos[i][j].sampler = g_sampler;
			ShadowingInfos[i][j].imageView = m_shadowMappingAttachments[i][j].view;
			ShadowingInfos[i][j].imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
		}
		Writes[i + Min].descriptorCount = MAX_LIGHT_COUNT;
		Writes[i + Min].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		Writes[i + Min].pImageInfo = ShadowingInfos[i];
	}
	
	const VkDescriptorBufferInfo BufferInfo = {
		.buffer = m_graphicsLightBuffer,
		.offset = 0,
		.range = VK_WHOLE_SIZE
	};
	Writes[Min + 1].descriptorCount = 1;
	Writes[Min + 1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	Writes[Min + 1].pBufferInfo = &BufferInfo;
	
	vkUpdateDescriptorSets(m_logical, DEFERRED_RENDERING_BINDING_COUNT, Writes, 0, nullptr);
	
	for(uint32_t i = 0; i < DEFERRED_RENDERING_REQUIRED_TEXTURE_COUNT; i++) delete[] TextureInfos[i];
}

void GPUFixedContext::unset_deferredRenderingBindings(void) {
	vkDestroySampler(m_logical, g_sampler, nullptr);
}



#endif
