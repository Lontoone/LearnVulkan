#pragma once
#include "CoreInstance.hpp"
#include "SwapChain.hpp"
#include "../utli/file_loader.hpp"
#include <iostream>
#include <filesystem>
#include "../utli/structure.hpp"


namespace ltn{
    class GraphicsPipeline{
    public:
        GraphicsPipeline(CoreInstance& _core , SwapChain& swapchain );
		~GraphicsPipeline();
		void load_shaders();

		void create_pipleine(VkRenderPass renderpass,
			std::vector<VkDescriptorSetLayout>* descriptors);
		inline VkPipeline get_pipeline() {return m_graphicsPipeline;};
		inline auto get_layout() { return m_pipeline_layout; };

		void cleanup();
    private:
		CoreInstance& m_core_instance;
		SwapChain& m_swapchain;

		VkShaderModule m_vert_shader_module;
		VkShaderModule m_frag_shader_module;
		VkPipelineLayout m_pipeline_layout;		
		VkPipeline m_graphicsPipeline;
		


		VkShaderModule createShaderModule(const std::vector<char>& code);

    };
}