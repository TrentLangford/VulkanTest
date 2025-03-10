#pragma once
#include "tv_device.hpp"

// std
#include <string>
#include <vector>

namespace tv
{
	// Stores all data on the graphics pipeline layout and settings
	struct PipelineConfigInfo 
	{
		// We might need these later to prevent memory badness
		// Still a little new to the c++ memory managing shenanigans
		/*PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo & operator=(const PipelineConfigInfo&) = delete;*/

		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};
	class TvPipeline
	{
	public:
		TvPipeline(TvDevice& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo info);
		~TvPipeline();

		// Remove those pesky copy operators
		TvPipeline(const TvPipeline&) = delete;
		void operator=(const TvPipeline&) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo, uint32_t width, uint32_t height);
	private:
		// reads a file as bytes (for reading compiled shader code)
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo info);
		void createShaderModule(const std::vector<char>& code, VkShaderModule* module);

		// our device class
		TvDevice& tvDevice;
		// vulcan pipeline class
		VkPipeline graphicsPipeline;
		// the compiled vert and frag shader modules
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}
