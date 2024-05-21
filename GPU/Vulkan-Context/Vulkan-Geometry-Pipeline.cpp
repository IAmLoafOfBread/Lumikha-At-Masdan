#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



#define VERTEX_MODULE_PATH "Geometry.vert.spv"
#define FRAGMENT_MODULE_PATH "Geometry.frag.spv"

static VkShaderModule g_vertexModule = VK_NULL_HANDLE;
static VkShaderModule g_fragmentModule = VK_NULL_HANDLE;


void GPUFixedContext::build_geometryPipeline(void) {
	build_module(g_vertexModule, VERTEX_MODULE_PATH);
	build_module(g_fragmentModule, FRAGMENT_MODULE_PATH);

	{
		const VkPushConstantRange PushConstant = {
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			.offset = 0,
			.size = sizeof(View)
		};
		const VkPipelineLayoutCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = 1,
			.pSetLayouts = &m_geometryDescriptorLayout,
			.pushConstantRangeCount = 1,
			.pPushConstantRanges = &PushConstant
		};
		CHECK(vkCreatePipelineLayout(m_logical, &CreateInfo, nullptr, &m_geometryLayout))
	}
	
	
	const VkPipelineShaderStageCreateInfo ShaderStages[] = {
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = g_vertexModule,
			.pName = "output_vertex\0",
			.pSpecializationInfo = nullptr
		},
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = g_fragmentModule,
			.pName = "output_fragment\0",
			.pSpecializationInfo = nullptr
		}
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
	};
	for(uint32_t i = 0; i < SIZE_OF(Attributes); i++) {
		Attributes[i].location = i;
	}
	
	const VkPipelineVertexInputStateCreateInfo VertexInputState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.vertexBindingDescriptionCount = SIZE_OF(Bindings),
		.pVertexBindingDescriptions = Bindings,
		.vertexAttributeDescriptionCount = SIZE_OF(Attributes),
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
	
	VkPipelineColorBlendAttachmentState ColourAttachments[GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT] = { 0 };
	for(uint32_t i = 0; i < GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT; i++) {
		ColourAttachments[i].blendEnable = VK_FALSE;
		ColourAttachments[i].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		ColourAttachments[i].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		ColourAttachments[i].colorBlendOp = VK_BLEND_OP_ADD;
		ColourAttachments[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		ColourAttachments[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		ColourAttachments[i].alphaBlendOp = VK_BLEND_OP_ADD;
		ColourAttachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	};
	
	const VkPipelineColorBlendStateCreateInfo ColourBlendState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_CLEAR,
		.attachmentCount = GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT,
		.pAttachments = ColourAttachments,
		.blendConstants = { 0 }
	};
	
	const VkPipelineDynamicStateCreateInfo DynamicState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.dynamicStateCount = 0,
		.pDynamicStates = nullptr
	};
	
	const VkGraphicsPipelineCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stageCount = SIZE_OF(ShaderStages),
		.pStages = ShaderStages,
		.pVertexInputState = &VertexInputState,
		.pInputAssemblyState = &InputAssemblyState,
		.pTessellationState = &TessellationState,
		.pViewportState = &ViewportState,
		.pRasterizationState = &RasterizationState,
		.pMultisampleState = &MultisampleState,
		.pDepthStencilState = &DepthStencilState,
		.pColorBlendState = &ColourBlendState,
		.pDynamicState = &DynamicState,
		.layout = m_geometryLayout,
		.renderPass = m_geometryPass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};
	CHECK(vkCreateGraphicsPipelines(m_logical, nullptr, 1, &CreateInfo, nullptr, &m_geometryPipeline))
}

void GPUFixedContext::ruin_geometryPipeline(void) {
	vkDestroyPipeline(m_logical, m_geometryPipeline, nullptr);
	vkDestroyPipelineLayout(m_logical, m_geometryLayout, nullptr);
	ruin_module(g_vertexModule);
	ruin_module(g_fragmentModule);
}



#endif
