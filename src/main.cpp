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

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	
	ImGui::StyleColorsLight();
		

	//1: create descriptor pool for IMGUI
	// the size of the pool is very oversize, but it's copied from imgui demo itself.
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	VkDescriptorPool imguiPool;
	vkCreateDescriptorPool(coreInstance.get_device(), &pool_info, nullptr, &imguiPool);


	// Initialize ImGui for GLFW and Vulkan
	ImGui_ImplGlfw_InitForVulkan(main_window.get_window(), true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = coreInstance.get_instance();
	init_info.PhysicalDevice = coreInstance.get_physical_device();
	init_info.Device = coreInstance.get_device();
	init_info.QueueFamily = coreInstance.get_queuefailmy_indexs()->graphic_queuefamily_index.value();
	init_info.Queue = coreInstance.graphic_queue();
	init_info.PipelineCache = nullptr;
	init_info.DescriptorPool = imguiPool;
	init_info.RenderPass = forward_renderer_pass->get_renderPass();
	init_info.Subpass = 0;
	init_info.MinImageCount = swapchain->MAX_FRAMES_IN_FLIGHT;
	init_info.ImageCount = swapchain->MAX_FRAMES_IN_FLIGHT;;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator = nullptr;
	init_info.CheckVkResultFn = nullptr;
	ImGui_ImplVulkan_Init(&init_info);

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
		ImGui::Text("hi2");
		ImGui::End();
		//ImGui::ShowDemoWindow();
		
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
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), forward_renderer_pass->get_current_cmdbuffer());


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

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}