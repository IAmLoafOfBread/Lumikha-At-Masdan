#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



#define MODULE_PATH "Light-Viewing.comp.spv"

static VkShaderModule g_module = VK_NULL_HANDLE;



void GPUFixedContext::build_lightViewingPipeline(void) {
	build_module(g_module, MODULE_PATH);
	
	{
		const VkPushConstantRange PushConstant = {
			.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
			.offset = 0,
			.size = (sizeof(float3) * CASCADED_SHADOW_MAP_COUNT * CORNER_COUNT) + sizeof(VkDeviceAddress) + sizeof(uint32_t)
		};
		const VkPipelineLayoutCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = 0,
			.pSetLayouts = nullptr,
			.pushConstantRangeCount = 1,
			.pPushConstantRanges = &PushConstant
		};
		CHECK(vkCreatePipelineLayout(m_logical, &CreateInfo, nullptr, &m_lightViewingLayout))
	}
	
	const VkComputePipelineCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stage = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_COMPUTE_BIT,
			.module = g_module,
			.pName = "view_light\0",
			.pSpecializationInfo = nullptr
		},
		.layout = m_lightViewingLayout,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};
	CHECK(vkCreateComputePipelines(m_logical, nullptr, 1, &CreateInfo, nullptr, &m_lightViewingPipeline))
}

void GPUFixedContext::ruin_lightViewingPipeline(void) {
	vkDestroyPipeline(m_logical, m_lightViewingPipeline, nullptr);
	vkDestroyPipelineLayout(m_logical, m_lightViewingLayout, nullptr);
	ruin_module(g_module);
}



#endif
