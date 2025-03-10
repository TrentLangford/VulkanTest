#include "tv_window.hpp"

// std
#include <stdexcept>

namespace tv
{
	TvWindow::TvWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name }
	{
		// call our private construction code
		initWindow();
	}

	TvWindow::~TvWindow()
	{
		// return glfw resoruces and terminate glfw upon deletion of the window
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void TvWindow::initWindow()
	{
		// standard glfw init stuff, but we hint for no api (no creation of opengl stuff) and no resizing
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}

	void TvWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		// creates a vulkan surface out of a vulkan instance and the current window
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}
}