#include "DisplayWindow.hpp"

ltn::DisplayWindow::DisplayWindow()
{
	init_window();
}

ltn::DisplayWindow::~DisplayWindow()
{
	destroy_resources();
}

void ltn::DisplayWindow::init_window()
{
	// Init window
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);


	this->window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "test window", nullptr, nullptr);
	glfwSetWindowUserPointer(window,this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

bool ltn::DisplayWindow::is_window_alive()
{
	return !glfwWindowShouldClose(this->window);
}

void ltn::DisplayWindow::destroy_resources()
{
	glfwDestroyWindow(this->window);
	//vkDestroySurfaceKHR(m_core_instance.get_instance(), m_surface, nullptr);
	glfwTerminate();
}

void ltn::DisplayWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	std::cout << "resize \n";
	auto ltnWindow = reinterpret_cast<DisplayWindow*>(glfwGetWindowUserPointer(window));
	ltnWindow->SCR_WIDTH = width;
	ltnWindow->SCR_HEIGHT = height;
	ltnWindow->frameBufferedResized = true;

}




