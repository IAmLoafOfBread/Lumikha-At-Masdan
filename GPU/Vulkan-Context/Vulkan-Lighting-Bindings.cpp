#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



#define BINDING_COUNT (GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT + CASCADED_SHADOW_MAP_COUNT)



void GPUFixedContext::build_lightingBindings(void) {
	{
		VkDescriptorSetLayoutBinding Bindings[BINDING_COUNT] = { { 0 } };
		for (uint32_t i = 0; i < BINDING_COUNT; i++) {
			Bindings[i].binding = i;
			Bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			Bindings[i].descriptorCount = 1;
			Bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			Bindings[i].pImmutableSamplers = nullptr;
		}
		for (uint32_t i = GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT; i < BINDING_COUNT; i++) {
			Bindings[i].descriptorCount = MAX_LIGHT_COUNT;
		}

		const VkDescriptorSetLayoutCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = BINDING_COUNT,
			.pBindings = Bindings
		};
		CHECK(vkCreateDescriptorSetLayout(m_logical, &CreateInfo, nullptr, &m_lightingDescriptorLayout))
	}

	{
		const VkDescriptorSetAllocateInfo AllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = m_graphicsBindingPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &m_lightingDescriptorLayout
		};
		CHECK(vkAllocateDescriptorSets(m_logical, &AllocInfo, &m_lightingDescriptorSet))
	}
}

void GPUFixedContext::ruin_lightingBindings(void) {
	CHECK(vkFreeDescriptorSets(m_logical, m_graphicsBindingPool, 1, &m_lightingDescriptorSet))
	vkDestroyDescriptorSetLayout(m_logical, m_lightingDescriptorLayout, nullptr);
}



#endif
