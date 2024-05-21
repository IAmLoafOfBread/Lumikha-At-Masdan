#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_geometryBindings(void) {
	{
		VkDescriptorSetLayoutBinding Bindings[GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT] = { { 0 } };
		for(uint32_t i = 0; i < GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT; i++) {
			Bindings[i].binding = i;
			Bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			Bindings[i].descriptorCount = m_meshCount;
			Bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			Bindings[i].pImmutableSamplers = nullptr;
		}
		
		const VkDescriptorSetLayoutCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT,
			.pBindings = Bindings
		};
		CHECK(vkCreateDescriptorSetLayout(m_logical, &CreateInfo, nullptr, &m_geometryDescriptorLayout))
	}
	
	{
		const VkDescriptorSetAllocateInfo AllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = m_graphicsBindingPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &m_geometryDescriptorLayout
		};
		CHECK(vkAllocateDescriptorSets(m_logical, &AllocInfo, &m_geometryDescriptorSet))
	}
}

void GPUFixedContext::ruin_geometryBindings(void) {
	CHECK(vkFreeDescriptorSets(m_logical, m_graphicsBindingPool, 1, &m_geometryDescriptorSet))
	vkDestroyDescriptorSetLayout(m_logical, m_geometryDescriptorLayout, nullptr);
}



#endif
