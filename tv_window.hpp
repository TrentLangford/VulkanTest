#pragma once

// GLFW/Vulkan
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// std
#include <string>

namespace tv
{
	class TvWindow
	{
	public:
		// constructs a window with width, height, and name
		TvWindow(int w, int h, std::string name);
		~TvWindow();

		// removing the copy operators to prevent memory fuckery
		TvWindow(const TvWindow&) = delete;
		TvWindow& operator = (const TvWindow &) = delete;

		// returns true if GLFW thinks the window should close (user dismisses the window)
		bool shouldClose() { return glfwWindowShouldClose(window); }
		// creates a Vulkan window surface
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		// performs the actual init for the window
		void initWindow();
		const int width, height;
		std::string windowName;

		// private reference to the GLFW window
		GLFWwindow *window;
	};
}