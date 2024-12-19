#include "./core/DisplayWindow.hpp"
#include "./core/CoreInstance.hpp"
#include "./core/SwapChain.hpp"
#include "./core/GraphicsPipeline.hpp"
#include "render/Renderer.hpp"
#include "imgui.h" 
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
int main() {
	ltn::DisplayWindow main_window{};
	ltn::CoreInstance coreInstance{ *main_window.get_window() };
	std::unique_ptr<ltn::SwapChain> swapchain = std::make_unique<ltn::SwapChain>(coreInstance,main_window.SCR_WIDTH , main_window.SCR_HEIGHT);
	std::unique_ptr<ltn::GraphicsPipeline> pipeline = std::make_unique<ltn::GraphicsPipeline>( coreInstance , *swapchain );
	std::unique_ptr<ltn::Renderer>forward_renderer_pass = std::make_unique<ltn::Renderer>( coreInstance , *swapchain );


	pipeline->create_pipleine(
		forward_renderer_pass->get_renderPass(),
		nullptr
		//gameobject.get_all_descriptorLayouts()
	);
	while (main_window.is_window_alive())
	{
		glfwPollEvents();

		// Detect resize :
		if (main_window.frameBufferedResized) {
			main_window.frameBufferedResized = false;
			vkDeviceWaitIdle(coreInstance.get_device());
			swapchain->cleanup();
			forward_renderer_pass->cleanup();
			
			swapchain = std::make_unique<ltn::SwapChain>(coreInstance, main_window.SCR_WIDTH, main_window.SCR_HEIGHT);
			forward_renderer_pass = std::make_unique<ltn::Renderer>(coreInstance, *swapchain);
			pipeline = std::make_unique<ltn::GraphicsPipeline>(coreInstance, *swapchain);
			pipeline->create_pipleine(
				forward_renderer_pass->get_renderPass(),
				nullptr
				//gameobject.get_all_descriptorLayouts()
			);
		}

		forward_renderer_pass->reset_renderpass();
		forward_renderer_pass->begin_commandBuffer();

		//===========================
		//		Draw commands
		//===========================
		vkCmdBindPipeline(
			forward_renderer_pass->get_current_cmdbuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS, 
			pipeline->get_pipeline());
		/*
		VkViewport viewport;
		viewport.x = 0; 
		viewport.y = 0;
		viewport.width = main_window.SCR_WIDTH;
		viewport.height= main_window.SCR_HEIGHT;
		viewport.minDepth = 0;
		viewport.maxDepth = 1;
		vkCmdSetViewport(forward_renderer_pass->get_current_cmdbuffer() , 0 , 1 , &viewport);

		VkRect2D scissor;
		scissor.offset = {0,0};
		scissor.extent = { static_cast<unsigned int>( main_window.SCR_WIDTH) , static_cast<unsigned int>(main_window.SCR_HEIGHT) };
		vkCmdSetScissor(forward_renderer_pass->get_current_cmdbuffer() , 0 , 1 , &scissor);
		*/

		vkCmdDraw(forward_renderer_pass->get_current_cmdbuffer(), 3, 1, 0, 0);
		//------------------------------

		forward_renderer_pass->end_render();
		forward_renderer_pass->draw_frame();
	}

	vkDeviceWaitIdle(coreInstance.get_device());
	//
	swapchain->cleanup();
	forward_renderer_pass->cleanup();
}