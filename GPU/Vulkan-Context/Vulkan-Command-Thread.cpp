#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_commandThread(uint32_t in_familyIndex, GPUCommandPool* in_pool, GPUCommandSet* in_set) {
	{
		const VkCommandPoolCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = in_familyIndex
		};
		CHECK(vkCreateCommandPool(m_logical, &CreateInfo, nullptr, in_pool))
	}
	const VkCommandBufferAllocateInfo AllocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = *in_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};
	CHECK(vkAllocateCommandBuffers(m_logical, &AllocInfo, in_set))
}

void GPUFixedContext::ruin_commandThread(GPUCommandPool* in_pool, GPUCommandSet* in_set) {
	vkFreeCommandBuffers(m_logical, *in_pool, 1, in_set);
	vkDestroyCommandPool(m_logical, *in_pool, nullptr);
}



#endif
