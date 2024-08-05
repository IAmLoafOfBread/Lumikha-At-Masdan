#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_postProcessingBindings(void) {
	{
		VkDescriptorSetLayoutBinding Bindings[POST_PROCESSING_BINDING_COUNT] = { { 0 } };
		for (uint32_t i = 0; i < POST_PROCESSING_BINDING_COUNT; i++) {
			Bindings[i].binding = i;
			Bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			Bindings[i].descriptorCount = 1;
			Bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			Bindings[i].pImmutableSamplers = nullptr;
		}
		
		const VkDescriptorSetLayoutCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = POST_PROCESSING_BINDING_COUNT,
			.pBindings = Bindings
		};
		CHECK(vkCreateDescriptorSetLayout(m_logical, &CreateInfo, nullptr, &m_postProcessingDescriptorLayout))
	}
	
	{
		const VkDescriptorSetAllocateInfo AllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = m_graphicsBindingPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &m_postProcessingDescriptorLayout
		};
		CHECK(vkAllocateDescriptorSets(m_logical, &AllocInfo, &m_postProcessingDescriptorSet))
	}
}

void GPUFixedContext::ruin_postProcessingBindings(void) {
	CHECK(vkFreeDescriptorSets(m_logical, m_graphicsBindingPool, 1, &m_postProcessingDescriptorSet))
	vkDestroyDescriptorSetLayout(m_logical, m_postProcessingDescriptorLayout, nullptr);
}



#endif
