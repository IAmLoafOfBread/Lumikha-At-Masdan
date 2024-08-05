#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_graphicsBindingPool(uint32_t in_meshCount) {
	m_meshCount = in_meshCount;

	const VkDescriptorPoolSize PoolSize = {
		.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.descriptorCount = (in_meshCount * GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT) + GEOMETRY_PASS_TOTAL_ATTACHMENT_COUNT + (MAX_LIGHT_COUNT * CASCADED_SHADOW_MAP_COUNT) + POST_PROCESSING_BINDING_COUNT
	};

	const VkDescriptorPoolCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		.maxSets = 3,
		.poolSizeCount = 1,
		.pPoolSizes = &PoolSize
	};
	CHECK(vkCreateDescriptorPool(m_logical, &CreateInfo, nullptr, &m_graphicsBindingPool))
}

void GPUFixedContext::ruin_graphicsBindingPool(void) {
	vkDestroyDescriptorPool(m_logical, m_graphicsBindingPool, nullptr);
}



#endif
