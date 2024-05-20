#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static VkDescriptorPool g_pool = VK_NULL_HANDLE;



void GPUFixedContext::build_deferredRenderingBindings(uint32_t in_meshCount) {
	m_meshCount = in_meshCount;
	
	{
		const VkDescriptorPoolSize PoolSizes[] = {
			{
				.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
				.descriptorCount = LIGHTING_PASS_INPUT_COUNT
			},
			{
				.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = (in_meshCount * DEFERRED_RENDERING_REQUIRED_TEXTURE_COUNT) + (MAX_LIGHT_COUNT * CASCADED_SHADOW_MAP_COUNT)
			},
			{
				.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1
			}
			
		};
		
		const VkDescriptorPoolCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.maxSets = 1,
			.poolSizeCount = SIZE_OF(PoolSizes),
			.pPoolSizes = PoolSizes
		};
		CHECK(vkCreateDescriptorPool(m_logical, &CreateInfo, nullptr, &g_pool))
	}
	
	{
		VkDescriptorSetLayoutBinding Bindings[DEFERRED_RENDERING_BINDING_COUNT] = { { 0 } };
		for(uint32_t i = 0; i < DEFERRED_RENDERING_BINDING_COUNT; i++) {
			Bindings[i].binding = i;
			Bindings[i].pImmutableSamplers = nullptr;
			Bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		uint32_t Min = 0;
		uint32_t Cap = LIGHTING_PASS_INPUT_COUNT;
		for(uint32_t i = Min; i < Cap; i++) {
			Bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			Bindings[i].descriptorCount = 1;
		}
		Min = Cap;
		Cap += DEFERRED_RENDERING_REQUIRED_TEXTURE_COUNT;
		for(uint32_t i = Min; i < Cap; i++) {
			Bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			Bindings[i].descriptorCount = in_meshCount;
		}
		Min = Cap;
		Cap += CASCADED_SHADOW_MAP_COUNT;
		for(uint32_t i = Min; i < Cap; i++) {
			Bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			Bindings[i].descriptorCount = MAX_LIGHT_COUNT;
		}
		Min = Cap;
		Cap += 1;
		for(uint32_t i = Min; i < Cap; i++) {
			Bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			Bindings[i].descriptorCount = 1;
		}
		
		const VkDescriptorSetLayoutCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = DEFERRED_RENDERING_BINDING_COUNT,
			.pBindings = Bindings
		};
		CHECK(vkCreateDescriptorSetLayout(m_logical, &CreateInfo, nullptr, &m_deferredRenderingDescriptorLayout))
	}
	
	{
		const VkDescriptorSetAllocateInfo AllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = g_pool,
			.descriptorSetCount = 1,
			.pSetLayouts = &m_deferredRenderingDescriptorLayout
		};
		CHECK(vkAllocateDescriptorSets(m_logical, &AllocInfo, &m_deferredRenderingDescriptorSet))
	}
}

void GPUFixedContext::ruin_deferredRenderingBindings(void) {
	CHECK(vkFreeDescriptorSets(m_logical, g_pool, 1, &m_deferredRenderingDescriptorSet))
	vkDestroyDescriptorSetLayout(m_logical, m_deferredRenderingDescriptorLayout, nullptr);
	vkDestroyDescriptorPool(m_logical, g_pool, nullptr);
}



#endif
