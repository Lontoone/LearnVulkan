#pragma once
#include "../core/Component.hpp"
#include "../core/CoreInstance.hpp"
#include "../core/SwapChain.hpp"
namespace ltn {
	class Renderer : public Component {
	public:
		Renderer(ltn::CoreInstance& core_instance, ltn::SwapChain& swapchain);
		~Renderer();

		// Render
		void reset_renderpass();
		void begin_commandBuffer();
		void end_render();
		void draw_frame();

		// Get / Set
		inline const auto get_renderPass()const { return m_renderpass; } // Todo : split to small class
		inline VkCommandBuffer& get_current_cmdbuffer() { return m_commandBuffers[m_swapchain.current_frame()]; }

		// End
		void cleanup();
	private:
		CoreInstance& m_core_instance;
		SwapChain& m_swapchain;
		VkRenderPass m_renderpass;
		std::vector<VkFramebuffer> m_swapChain_framebuffers;
		std::vector<VkCommandBuffer> m_commandBuffers;

		// Init
		void create_frameBuffer(SwapChain& swapchain, VkRenderPass renderPass);
		void create_commandBuffer();
		void create_renderPass();

		
		uint32_t m_imageIndex;


	};

}