#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



#define GEOMETRY_VERTEX_MODULE_PATH "Geometry.vert.spv"
#define GEOMETRY_FRAGMENT_MODULE_PATH "Geometry.frag.spv"
#define LIGHTING_VERTEX_MODULE_PATH "Lighting.vert.spv"
#define LIGHTING_FRAGMENT_MODULE_PATH "Lighting.frag.spv"

static VkShaderModule g_geometryVertexModule = VK_NULL_HANDLE;
static VkShaderModule g_geometryFragmentModule = VK_NULL_HANDLE;
static VkShaderModule g_lightingVertexModule = VK_NULL_HANDLE;
static VkShaderModule g_lightingFragmentModule = VK_NULL_HANDLE;


void GPUFixedContext::build_deferredRenderingPipelines(void) {
	{
		const VkPushConstantRange PushConstants[] = {
			{
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
				.offset = 0,
				.size = sizeof(View)
			},
			{
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
				.offset = sizeof(View),
				.size = sizeof(uint32_t)
			}
		};
		const VkPipelineLayoutCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = 1,
			.pSetLayouts = &m_deferredRenderingDescriptorLayout,
			.pushConstantRangeCount = SIZE_OF(PushConstants),
			.pPushConstantRanges = PushConstants
		};
		CHECK(vkCreatePipelineLayout(m_logical, &CreateInfo, nullptr, &m_deferredRenderingLayout))
		build_module(g_geometryVertexModule, GEOMETRY_VERTEX_MODULE_PATH);
		build_module(g_geometryFragmentModule, GEOMETRY_FRAGMENT_MODULE_PATH);
		build_module(g_lightingVertexModule, LIGHTING_VERTEX_MODULE_PATH);
		build_module(g_lightingFragmentModule, LIGHTING_FRAGMENT_MODULE_PATH);
	}
	
	
	const VkPipelineShaderStageCreateInfo ShaderStages[2][2] = {
		{
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stage = VK_SHADER_STAGE_VERTEX_BIT,
				.module = g_geometryVertexModule,
				.pName = "main\0",
				.pSpecializationInfo = nullptr
			},
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
				.module = g_geometryFragmentModule,
				.pName = "main\0",
				.pSpecializationInfo = nullptr
			}
		},
		{
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stage = VK_SHADER_STAGE_VERTEX_BIT,
				.module = g_lightingVertexModule,
				.pName = "main\0",
				.pSpecializationInfo = nullptr
			},
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
				.module = g_lightingFragmentModule,
				.pName = "main\0",
				.pSpecializationInfo = nullptr
			}
		}
	};
	
	const VkVertexInputBindingDescription GeometryBindings[] = {
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
	
	VkVertexInputAttributeDescription GeometryAttributes[] = {
		{
			.binding = 0,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = offsetof(Vertex, position)
		},
		{
			.binding = 0,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = offsetof(Vertex, normal)
		},
		{
			.binding = 0,
			.format = VK_FORMAT_R32G32_SFLOAT,
			.offset = offsetof(Vertex, uv)
		},
		{
			.binding = 1,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = offsetof(Instance, position)
		},
		{
			.binding = 1,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = offsetof(Instance, rotation)
		},
		{
			.binding = 1,
			.format = VK_FORMAT_R32_UINT,
			.offset = offsetof(Instance, textureIndex)
		}
	};
	for(uint32_t i = 0; i < SIZE_OF(GeometryAttributes); i++) {
		GeometryAttributes[i].location = i;
	}
	
	const VkPipelineVertexInputStateCreateInfo VertexInputStates[2] = {
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = SIZE_OF(GeometryBindings),
			.pVertexBindingDescriptions = GeometryBindings,
			.vertexAttributeDescriptionCount = SIZE_OF(GeometryAttributes),
			.pVertexAttributeDescriptions = GeometryAttributes
		},
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = 0,
			.pVertexBindingDescriptions = nullptr,
			.vertexAttributeDescriptionCount = 0,
			.pVertexAttributeDescriptions = nullptr
		}
	};
	
	const VkPipelineInputAssemblyStateCreateInfo InputAssemblyStates[2] = {
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		},
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
			.primitiveRestartEnable = VK_FALSE
		}
	};
	
	const VkPipelineTessellationStateCreateInfo TessellationState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.patchControlPoints = 1
	};
	
	const VkViewport MainViewport = {
		.x = 0,
		.y = 0,
		.width = static_cast<float>(m_surfaceExtent.width),
		.height = static_cast<float>(m_surfaceExtent.height),
		.minDepth = 0,
		.maxDepth = 1
	};
	const VkRect2D MainScissor = {
		.offset = {0},
		.extent = {m_surfaceExtent.width, m_surfaceExtent.height}
	};
	const VkPipelineViewportStateCreateInfo ViewportState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.viewportCount = 1,
		.pViewports = &MainViewport,
		.scissorCount = 1,
		.pScissors = &MainScissor,
	};
	
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
		.minSampleShading = 1,
		.pSampleMask = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE,
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
	
	const VkPipelineColorBlendAttachmentState ColourAttachment = {
		.blendEnable = VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};
	VkPipelineColorBlendAttachmentState GeometryColourAttachments[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT] = { { 0 } };
	for(uint32_t i = 0; i < GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT; i++) GeometryColourAttachments[i] = ColourAttachment;
	
	const VkPipelineColorBlendStateCreateInfo ColourBlendStates[2] = {
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT,
			.pAttachments = GeometryColourAttachments,
			.blendConstants = { 0 }
		},
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = 1,
			.pAttachments = &ColourAttachment,
			.blendConstants = { 0 }
		}
	};
	
	const VkPipelineDynamicStateCreateInfo DynamicState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.dynamicStateCount = 0,
		.pDynamicStates = nullptr
	};
	
	VkGraphicsPipelineCreateInfo CreateInfos[2] = { {  } };
	for(uint32_t i = 0; i < 2; i++) {
		CreateInfos[i].sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		CreateInfos[i].pNext = nullptr;
		CreateInfos[i].flags = 0;
		CreateInfos[i].stageCount = 2;
		CreateInfos[i].pStages = &ShaderStages[i][0];
		CreateInfos[i].pVertexInputState = &VertexInputStates[i];
		CreateInfos[i].pInputAssemblyState = &InputAssemblyStates[i];
		CreateInfos[i].pTessellationState = &TessellationState;
		CreateInfos[i].pViewportState = &ViewportState;
		CreateInfos[i].pRasterizationState = &RasterizationState;
		CreateInfos[i].pMultisampleState = &MultisampleState;
		CreateInfos[i].pDepthStencilState = i == 0 ? &DepthStencilState : nullptr;
		CreateInfos[i].pColorBlendState = &ColourBlendStates[i];
		CreateInfos[i].pDynamicState = &DynamicState;
		CreateInfos[i].layout = m_deferredRenderingLayout;
		CreateInfos[i].renderPass = m_deferredRenderingPass;
		CreateInfos[i].subpass = i;
		CreateInfos[i].basePipelineHandle = VK_NULL_HANDLE;
		CreateInfos[i].basePipelineIndex = -1;
	}
	
	CHECK(vkCreateGraphicsPipelines(m_logical, nullptr, 2, CreateInfos, nullptr, m_deferredRenderingPipelines))
}

void GPUFixedContext::ruin_deferredRenderingPipelines(void) {
	vkDestroyPipeline(m_logical, m_deferredRenderingPipelines[0], nullptr);
	vkDestroyPipeline(m_logical, m_deferredRenderingPipelines[1], nullptr);
	ruin_module(g_geometryVertexModule);
	ruin_module(g_geometryFragmentModule);
	ruin_module(g_lightingVertexModule);
	ruin_module(g_lightingFragmentModule);
	vkDestroyPipelineLayout(m_logical, m_deferredRenderingLayout, nullptr);
}



#endif
