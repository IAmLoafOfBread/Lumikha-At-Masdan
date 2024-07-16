#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



#define VERTEX_MODULE_PATH "Lighting.vert.spv"
#define FRAGMENT_MODULE_PATH "Lighting.frag.spv"

static VkShaderModule g_vertexModule = VK_NULL_HANDLE;
static VkShaderModule g_fragmentModule = VK_NULL_HANDLE;


void GPUFixedContext::build_lightingPipeline(void) {
	build_module(&g_vertexModule, VERTEX_MODULE_PATH);
	build_module(&g_fragmentModule, FRAGMENT_MODULE_PATH);

	{
		const VkPushConstantRange PushConstant = {
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
			.offset = 0,
			.size = sizeof(float3) + sizeof(uint32_t) + sizeof(VkDeviceAddress)
		};
		const VkPipelineLayoutCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = 1,
			.pSetLayouts = &m_lightingDescriptorLayout,
			.pushConstantRangeCount = 1,
			.pPushConstantRanges = &PushConstant
		};
		CHECK(vkCreatePipelineLayout(m_logical, &CreateInfo, nullptr, &m_lightingLayout))
	}


	const VkPipelineShaderStageCreateInfo ShaderStages[] = {
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = g_vertexModule,
			.pName = "main\0",
			.pSpecializationInfo = nullptr
		},
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = g_fragmentModule,
			.pName = "main\0",
			.pSpecializationInfo = nullptr
		}
	};

	const VkPipelineVertexInputStateCreateInfo VertexInputState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.vertexBindingDescriptionCount = 0,
		.pVertexBindingDescriptions = nullptr,
		.vertexAttributeDescriptionCount = 0,
		.pVertexAttributeDescriptions = nullptr
	};

	const VkPipelineInputAssemblyStateCreateInfo InputAssemblyState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
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
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
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

	const VkPipelineColorBlendStateCreateInfo ColourBlendState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_CLEAR,
		.attachmentCount = 1,
		.pAttachments = &ColourAttachment,
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
		.stageCount = LENGTH_OF(ShaderStages),
		.pStages = ShaderStages,
		.pVertexInputState = &VertexInputState,
		.pInputAssemblyState = &InputAssemblyState,
		.pTessellationState = &TessellationState,
		.pViewportState = &ViewportState,
		.pRasterizationState = &RasterizationState,
		.pMultisampleState = &MultisampleState,
		.pDepthStencilState = nullptr,
		.pColorBlendState = &ColourBlendState,
		.pDynamicState = &DynamicState,
		.layout = m_lightingLayout,
		.renderPass = m_lightingPass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};
	CHECK(vkCreateGraphicsPipelines(m_logical, nullptr, 1, &CreateInfo, nullptr, &m_lightingPipeline))
}

void GPUFixedContext::ruin_lightingPipeline(void) {
	vkDestroyPipeline(m_logical, m_lightingPipeline, nullptr);
	vkDestroyPipelineLayout(m_logical, m_lightingLayout, nullptr);
	ruin_module(g_vertexModule);
	ruin_module(g_fragmentModule);
}



#endif
