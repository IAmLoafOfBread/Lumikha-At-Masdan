#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



#define VERTEX_MODULE_PATH "Shadow-Mapping.vert.spv"

static VkShaderModule g_vertexModule = VK_NULL_HANDLE;



void GPUFixedContext::build_shadowMappingPipelines(void) {
	build_module(&g_vertexModule, VERTEX_MODULE_PATH);
	
	{
		const VkPushConstantRange PushConstant = {
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			.offset = 0,
			.size = sizeof(View) + sizeof(float3)
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
		CHECK(vkCreatePipelineLayout(m_logical, &CreateInfo, nullptr, &m_shadowMappingLayout))
	}
	
	
	const VkPipelineShaderStageCreateInfo ShaderStage = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = g_vertexModule,
		.pName = "main",
		.pSpecializationInfo = nullptr
	};
	
	const VkVertexInputBindingDescription Bindings[] = {
		{
			.binding = 0,
			.stride = sizeof(Vertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		},
		{
			.binding = 1,
			.stride = sizeof(Instance),
			.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE
		}
	};
	
	VkVertexInputAttributeDescription Attributes[] = {
		{
			.binding = 0,
			.offset = offsetof(Vertex, position)
		},
		{
			.binding = 1,
			.offset = offsetof(Instance, position)
		},
		{
			.binding = 1,
			.offset = offsetof(Instance, rotation)
		}
	};
	for(uint32_t i = 0; i < LENGTH_OF(Attributes); i++) {
		Attributes[i].location = i;
		Attributes[i].format = VK_FORMAT_R32G32B32_SFLOAT;
	}
	
	const VkPipelineVertexInputStateCreateInfo VertexInputState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.vertexBindingDescriptionCount = LENGTH_OF(Bindings),
		.pVertexBindingDescriptions = Bindings,
		.vertexAttributeDescriptionCount = LENGTH_OF(Attributes),
		.pVertexAttributeDescriptions = Attributes
	};
	
	const VkPipelineInputAssemblyStateCreateInfo InputAssemblyState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};
	
	const VkPipelineTessellationStateCreateInfo TessellationState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.patchControlPoints = 1
	};
	
	
	const VkExtent3D Extent = SHADOW_MAP_EXTENT;
	VkViewport MainViewports[CASCADED_SHADOW_MAP_COUNT] = { 0 };
	VkRect2D MainScissors[CASCADED_SHADOW_MAP_COUNT] = { 0 };
	VkPipelineViewportStateCreateInfo ViewportStates[CASCADED_SHADOW_MAP_COUNT] = { {  } };
	{
		uint32_t Divisor = 1;
		for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
			const uint32_t Width = Extent.width / Divisor;
			const uint32_t Height = Extent.height / Divisor;
			
			MainViewports[i].x = 0;
			MainViewports[i].y = 0;
			MainViewports[i].width = static_cast<float>(Width);
			MainViewports[i].height = static_cast<float>(Height);
			MainViewports[i].minDepth = 0;
			MainViewports[i].maxDepth = 1;
			
			MainScissors[i].offset.x = 0;
			MainScissors[i].offset.y = 0;
			MainScissors[i].extent.width = Width;
			MainScissors[i].extent.height = Height;
			
			ViewportStates[i].sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			ViewportStates[i].pNext = nullptr;
			ViewportStates[i].flags = 0;
			ViewportStates[i].viewportCount = 1;
			ViewportStates[i].pViewports = &MainViewports[i];
			ViewportStates[i].scissorCount = 1;
			ViewportStates[i].pScissors = &MainScissors[i];
			
			Divisor++;
		}
	}
	
	const VkPipelineRasterizationStateCreateInfo RasterizationState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0,
		.depthBiasClamp = 0,
		.depthBiasSlopeFactor = 0,
		.lineWidth = 1
	};

	const VkPipelineMultisampleStateCreateInfo MultisampleState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 0,
		.pSampleMask = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE
	};
	
	const VkPipelineDepthStencilStateCreateInfo DepthStencilState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_GREATER,
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable = VK_FALSE,
		.front = {
			.failOp = static_cast<VkStencilOp>(0),
			.passOp = static_cast<VkStencilOp>(0),
			.depthFailOp = static_cast<VkStencilOp>(0),
			.compareOp = static_cast<VkCompareOp>(0),
			.compareMask = 0,
			.writeMask = 0,
			.reference = 0
		},
			.back = {
				.failOp = static_cast<VkStencilOp>(0),
				.passOp = static_cast<VkStencilOp>(0),
				.depthFailOp = static_cast<VkStencilOp>(0),
				.compareOp = static_cast<VkCompareOp>(0),
				.compareMask = 0,
				.writeMask = 0,
				.reference = 0
			},
		.minDepthBounds = 0,
		.maxDepthBounds = 1,
	};
	
	const VkPipelineColorBlendStateCreateInfo ColourBlendState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.logicOpEnable = VK_FALSE,
		.logicOp = static_cast<VkLogicOp>(0),
		.attachmentCount = 0,
		.pAttachments = nullptr,
		.blendConstants = { 0 }
	};

	const VkPipelineDynamicStateCreateInfo DynamicState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.dynamicStateCount = 0,
		.pDynamicStates = nullptr
	};
	
	VkGraphicsPipelineCreateInfo CreateInfos[CASCADED_SHADOW_MAP_COUNT] = { {  } };
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		CreateInfos[i].sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		CreateInfos[i].pNext = nullptr;
		CreateInfos[i].flags = 0;
		CreateInfos[i].stageCount = 1;
		CreateInfos[i].pStages = &ShaderStage;
		CreateInfos[i].pVertexInputState = &VertexInputState;
		CreateInfos[i].pInputAssemblyState = &InputAssemblyState;
		CreateInfos[i].pTessellationState = &TessellationState;
		CreateInfos[i].pViewportState = &ViewportStates[i];
		CreateInfos[i].pRasterizationState = &RasterizationState;
		CreateInfos[i].pMultisampleState = &MultisampleState;
		CreateInfos[i].pDepthStencilState = &DepthStencilState;
		CreateInfos[i].pColorBlendState = &ColourBlendState;
		CreateInfos[i].pDynamicState = &DynamicState;
		CreateInfos[i].layout = m_shadowMappingLayout;
		CreateInfos[i].renderPass = m_shadowMappingPass;
		CreateInfos[i].subpass = 0;
		CreateInfos[i].basePipelineHandle = VK_NULL_HANDLE;
		CreateInfos[i].basePipelineIndex = -1;
	}
	CHECK(vkCreateGraphicsPipelines(m_logical, nullptr, CASCADED_SHADOW_MAP_COUNT, CreateInfos, nullptr, m_shadowMappingPipelines))
}

void GPUFixedContext::ruin_shadowMappingPipelines(void) {
	for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
		vkDestroyPipeline(m_logical, m_shadowMappingPipelines[i], nullptr);
	}
	vkDestroyPipelineLayout(m_logical, m_shadowMappingLayout, nullptr);
	ruin_module(g_vertexModule);
}



#endif
