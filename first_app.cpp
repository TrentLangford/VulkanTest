#include "first_app.hpp"

namespace tv
{
	void FirstApp::Run()
	{
		// Self explanatory while loop game programming here
		while (!tvWindow.shouldClose())
		{
			glfwPollEvents();
		}
	}
}