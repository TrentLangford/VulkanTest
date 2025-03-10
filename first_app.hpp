#pragma once

#include "tv_window.hpp"
#include "tv_pipeline.hpp"
#include "tv_device.hpp"


namespace tv
{
	// This app class contains the width and height data of the window, the run function, and three engine references
	class FirstApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		void Run();
	private:
		// The window object is what gets initially created and drawn to
		TvWindow tvWindow{ WIDTH, HEIGHT, "Hello Vulkan" };
		// the device object contains the logical object(?) of the drawing device, the gpu
		TvDevice tvDevice{ tvWindow };
		// the pipeline contains the information regarding how the engine renders, such as the vert and frag shaders and the layout of the pipeline itself
		TvPipeline tvPipeline{ tvDevice, "simple_shader.vert.spv", "simple_shader.frag.spv", TvPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
	};
}