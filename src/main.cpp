#define TINYOBJLOADER_IMPLEMENTATION
#include "./core/DisplayWindow.hpp"
#include "./core/CoreInstance.hpp"
#include "./core/SwapChain.hpp"
#include "./core/GraphicsPipeline.hpp"
#include "render/Renderer.hpp"
#include "./core/UiManager.hpp"
#include "./core/Model.hpp"
#include "./core/TransformObject.hpp"
#include "core/GameObject.hpp"
#include "./core/image.hpp"

int main() {
	// temp:
	const std::string MODEL_PATH = "./assets/models/viking_room.obj";
    const std::string TEXTURE_PATH = "./assets/models/viking_room.png";


	ltn::DisplayWindow main_window{};
	ltn::CoreInstance coreInstance{ *main_window.get_window() };
	std::unique_ptr<ltn::SwapChain> swapchain = std::make_unique<ltn::SwapChain>(coreInstance,main_window.SCR_WIDTH , main_window.SCR_HEIGHT);
	std::unique_ptr<ltn::GraphicsPipeline> pipeline = std::make_unique<ltn::GraphicsPipeline>( coreInstance , *swapchain );
	std::unique_ptr<ltn::Renderer>forward_renderer_pass = std::make_unique<ltn::Renderer>( coreInstance , *swapchain );
	
	ltn::GameObject gameobject{};
	ltn::TransformObject transform{ coreInstance, *pipeline };
	ltn::Model model{ coreInstance , *pipeline};
	ltn::Image img{ coreInstance };
	forward_renderer_pass->m_texture_image = &img;
	//img.load_texture("./assets/texture.jpg");
	img.load_texture(TEXTURE_PATH.c_str());
	model.load_model(MODEL_PATH.c_str());

	gameobject.add_component(&transform);
	gameobject.add_component(&model);
	gameobject.add_component(forward_renderer_pass.get());

	pipeline->create_pipleine(
		forward_renderer_pass->get_renderPass(),		
		gameobject.get_all_descriptorLayouts()
	);
	

		
	ltn::UiManager ui_manager{};
	ui_manager.initUi(main_window.get_window() , coreInstance.get_instance(), coreInstance.get_device() , coreInstance.get_physical_device() , coreInstance.get_queuefailmy_indexs()->graphic_queuefamily_index.value(),coreInstance.graphic_queue() , forward_renderer_pass->get_renderPass() , MAX_FRAMES_IN_FLIGHT);
	
	while (main_window.is_window_alive())
	{
		glfwPollEvents();

		// Start the ImGui frame 
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Hello, world!");
		ImGui::Text("hi");
		ImGui::End();
		ImGui::Begin("Hello2");
		ImGui::Text("fps %d " , ui_manager.get_run_time_fps());
		
		ImGui::End();
		
		// Detect resize :
		if (main_window.frameBufferedResized) {
			main_window.frameBufferedResized = false;
			gameobject.remove_component(forward_renderer_pass->index);
			vkDeviceWaitIdle(coreInstance.get_device());
			swapchain->cleanup();
			forward_renderer_pass->cleanup();
			
			swapchain = std::make_unique<ltn::SwapChain>(coreInstance, main_window.SCR_WIDTH, main_window.SCR_HEIGHT);
			forward_renderer_pass = std::make_unique<ltn::Renderer>(coreInstance, *swapchain);
			forward_renderer_pass->m_texture_image = &img;
			gameobject.add_component(forward_renderer_pass.get());
			/*
			pipeline = std::make_unique<ltn::GraphicsPipeline>(coreInstance, *swapchain);
			pipeline->create_pipleine(
				forward_renderer_pass->get_renderPass(),
				nullptr
				//gameobject.get_all_descriptorLayouts()
			);
			*/
			vkDeviceWaitIdle(coreInstance.get_device());
		}
		// collect update data
		ltn::FrameUpdateData update_data{
			swapchain->current_frame(),
			(float)main_window.SCR_WIDTH / (float)main_window.SCR_HEIGHT,
			forward_renderer_pass->get_current_cmdbuffer(),
			pipeline->get_layout()
		};

		forward_renderer_pass->reset_renderpass();
		forward_renderer_pass->begin_commandBuffer();

		//===========================
		//		Draw commands
		//===========================
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), forward_renderer_pass->get_current_cmdbuffer());

		gameobject.execute(update_data);
		//------------------------------

		forward_renderer_pass->end_render();
		forward_renderer_pass->draw_frame();

		ui_manager.update();
	}

	vkDeviceWaitIdle(coreInstance.get_device());
	//
	swapchain->cleanup();
	forward_renderer_pass->cleanup();

}