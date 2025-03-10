#pragma once

#include "tv_window.hpp"
#include "tv_pipeline.hpp"
#include "tv_device.hpp"
#include "tv_swap_chain.hpp"

// std
#include <memory>
#include <vector>

namespace tv
{
	// This app class contains the width and height data of the window, the run function, and three engine references
	class FirstApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator = (const FirstApp&) = delete;

		void Run();
	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		// The window object is what gets initially created and drawn to
		TvWindow tvWindow{ WIDTH, HEIGHT, "Hello Vulkan" };
		// the device object contains the logical object(?) of the drawing device, the gpu
		TvDevice tvDevice{ tvWindow };
		// the swapchain provides info on the buffering process/how the frame is presented
		TvSwapChain tvSwapChain{ tvDevice, tvWindow.getExtent() };
		// the pipeline contains the information regarding how the engine renders, such as the vert and frag shaders and the layout of the pipeline itself
		std::unique_ptr<TvPipeline> tvPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}