#include "tv_pipeline.hpp"

// std
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>

namespace tv
{
	TvPipeline::TvPipeline(TvDevice& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo info) : tvDevice{device}
	{
		// call our private constructor once again
		createGraphicsPipeline(vertFilepath, fragFilepath, info);
	}

	TvPipeline::~TvPipeline()
	{
		// make sure to destroy all the vulkan stuff when we destroy the pipeline
		vkDestroyShaderModule(tvDevice.device(), vertShaderModule, nullptr);
		vkDestroyShaderModule(tvDevice.device(), fragShaderModule, nullptr);
		vkDestroyPipeline(tvDevice.device(), graphicsPipeline, nullptr);
	}

	std::vector<char> TvPipeline::readFile(const std::string& filepath)
	{
		// Standard file programming stuff with added c++ fanciness
		// std::ios::ate moves to the end of the file, essentially handing us the filesize for later
		// very useful
		std::ifstream file{ filepath, std::ios::ate | std::ios::binary };
		if (!file.is_open())
		{
			// well shit can't open the file (you, yes you trent, fucked up somehow)
			throw std::runtime_error("failed to open file: " + filepath);
		}

		// get the filesize by reading the pos, cuz we ate that hoe
		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buf(fileSize);

		// standard file stuff, back to the start and read everything before closing our file like a good boy
		file.seekg(0);
		file.read(buf.data(), fileSize);
		file.close();

		return buf;
	}

	void TvPipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo configInfo)
	{
		// here's where imma attempt to explain things i don't understand

		// these asserts should obviously never be false unless you haven't finished the code or something fucked up
		assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
		assert(configInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo");

		// read our compiled shaders as bytes
		auto vertCode = readFile(vertFilepath);
		auto fragCode = readFile(fragFilepath);

		// create shader modules from bytes, attach it to the class
		createShaderModule(vertCode, &vertShaderModule);
		createShaderModule(fragCode, &fragShaderModule);

		// create the stage info for the vert and frag stage
		VkPipelineShaderStageCreateInfo shaderStages[2];

		// vert
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = vertShaderModule;
		shaderStages[0].pName = "main";		// entry point for the program
		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;

		// frag
		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = fragShaderModule;
		shaderStages[1].pName = "main";		// entry point for the program
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		// some extra input info for the vertex stage (idk why the frag doesn't also have one_
		VkPipelineVertexInputStateCreateInfo vertexInputInfo;
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;		// Should change once we arent hardcoding verts
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;

		// graphics pipeline create info used to create the graphics pipeline (duh)
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

		// bind the stage info
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;

		// bind the config info
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &configInfo.viewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		pipelineInfo.pDynamicState = nullptr;
		pipelineInfo.layout = configInfo.pipelineLayout;
		pipelineInfo.renderPass = configInfo.renderPass;
		pipelineInfo.subpass = configInfo.subpass;

		// some special sauce that we don't need
		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(tvDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
		{
			// Something went wrong, probably my fault but not as stupid of a mistake
			throw std::runtime_error("failed to create graphics pipeline");
		}
	}

	void TvPipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* module)
	{
		// creating shader module from byte array is surprisingly easy (it's already compiled ig)
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(tvDevice.device(), &createInfo, nullptr, module) != VK_SUCCESS)
		{
			// i'm not entirely sure how this would fail actually
			throw std::runtime_error("failed to create shader module");
		}
	}

	PipelineConfigInfo TvPipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height)
	{
		// here's the big one: the default config which will probably become the permanent config for a while
		PipelineConfigInfo configInfo{};

		// input assembly has to do with how vulkan interprets verticies and assebles triangles from them
		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;	// Every three verts is a triangle
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		// The viewport data, like the origin, w/h, depth clipping stuff (this might actually be fucked?)
		configInfo.viewport.x = 0.0f;
		configInfo.viewport.y = 0.0f;
		configInfo.viewport.width = static_cast<float>(width);
		configInfo.viewport.height = static_cast<float>(height);
		configInfo.viewport.minDepth = 0.0f;
		configInfo.viewport.maxDepth = 1.0f;

		// scissors used to clip the screen (not helpful rn)
		configInfo.scissor.offset = { 0, 0 };
		configInfo.scissor.extent = { width, height };

		// viewport info contains viewport
		configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		configInfo.viewportInfo.viewportCount = 1; // possible vr things here?
		configInfo.viewportInfo.pViewports = &configInfo.viewport;
		configInfo.viewportInfo.scissorCount = 1;
		configInfo.viewportInfo.pScissors = &configInfo.scissor;

		// info for the rasterization stage, pretty straightforward
		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterizationInfo.lineWidth = 1.0;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE; // find a way to make culling work eventually
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;		// determines how we know which tris are forward (neat)
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;	// Apparently optional (already disabled?)
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;				// ^^
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;		// ^^

		// info for multisampling, basically MSAA setup
		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;				// opt
		configInfo.multisampleInfo.pSampleMask = nullptr;				// opt
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;	// opt
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;			// opt

		// A default "empty"-ish color blend attachment we create to attach by default
		// color blending used when multiple colors are stored to a pixel (i.e. transparency)
		configInfo.colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;		// All below opt (photoshop blend settings?)
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		// color blend info contains all of the color blend attachments (disabled anyways)
		configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // opt
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

		// info on the depth stencil, which can probably be used for neat optimizaions if you are smart
		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;		// opt
		configInfo.depthStencilInfo.maxDepthBounds = 0.0f;		// opt
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.front = {};		// opt
		configInfo.depthStencilInfo.back = {};		// opt

		return configInfo;
	}
}