#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <iostream>

namespace ltn {
	class DisplayWindow {
	public:
		DisplayWindow();
		~DisplayWindow();
		int SCR_WIDTH = 480;
		int SCR_HEIGHT = 480;
		bool frameBufferedResized = false;
		void init_window();
		bool is_window_alive();
		

		void destroy_resources();
	

		//------------------
		//   Get / Set
		//------------------
		GLFWwindow* get_window() { return window; }
	

	private:
		GLFWwindow* window;	
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};

}