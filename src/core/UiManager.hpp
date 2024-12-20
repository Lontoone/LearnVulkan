#pragma once
#include "imgui.h" 
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"
#include <iostream>
namespace ltn {
	class UiManager
	{
	public:
		UiManager() {};
		~UiManager();

		void InitUi(
			GLFWwindow* window,
			VkInstance instance,
			VkDevice device,
			VkPhysicalDevice physical_device,
			uint32_t queue_family,
			VkQueue queue,
			VkRenderPass renderpass,
			int image_count
		);
	private:
		void cleanup();
	};

}