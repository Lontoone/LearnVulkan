#include "./core/DisplayWindow.hpp"
#include "./core/CoreInstance.hpp"
#include "./core/SwapChain.hpp"
#include "./core/GraphicsPipeline.hpp"
#include "render/Renderer.hpp"
int main() {
	ltn::DisplayWindow main_window{};
	ltn::CoreInstance coreInstance{ *main_window.get_window() };
	ltn::SwapChain swapchain{coreInstance,main_window.SCR_WIDTH , main_window.SCR_HEIGHT};
	ltn::GraphicsPipeline pipeline{ coreInstance , swapchain };
	ltn::Renderer forward_renderer_pass{ coreInstance , swapchain };


	pipeline.create_pipleine(
		forward_renderer_pass.get_renderPass(),
		nullptr
		//gameobject.get_all_descriptorLayouts()
	);
	while (main_window.is_window_alive())
	{
		glfwPollEvents();
		forward_renderer_pass.reset_renderpass();
		forward_renderer_pass.begin_commandBuffer();

		//===========================
		//		Draw commands
		//===========================
		vkCmdBindPipeline(
			forward_renderer_pass.get_current_cmdbuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS, 
			pipeline.get_pipeline());
		vkCmdDraw(forward_renderer_pass.get_current_cmdbuffer(), 3, 1, 0, 0);
		//------------------------------

		forward_renderer_pass.end_render();
		forward_renderer_pass.draw_frame();
	}
}