#pragma once
#include "imgui.h" 
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <chrono> // for calculating fps

namespace ltn {
	class UiManager
	{
	public:
		UiManager() {};
		~UiManager();

		void initUi(
			GLFWwindow* window,
			VkInstance instance,
			VkDevice device,
			VkPhysicalDevice physical_device,
			uint32_t queue_family,
			VkQueue queue,
			VkRenderPass renderpass,
			int image_count
		);

		void update();
		//===============================
		//			Get/Set
		//===============================
		inline int get_run_time_fps() { return m_fps; }
	private:
		void cleanup();
		void calculat_fps();

		// For calculating fps:
		//uint32_t m_frameCount = 0;		
		std::chrono::steady_clock::time_point m_fps_start_time;
		int m_fps;
	};

}