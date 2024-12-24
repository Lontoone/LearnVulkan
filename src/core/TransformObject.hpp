#pragma once
#include "../core/config.hpp"
#include "../utli/structure.hpp"
#include "vulkan/vulkan.h"
#include "../utli//helper_functions.hpp"
#include "../core/Component.hpp"
#include "../core/CoreInstance.hpp"
#include "GraphicsPipeline.hpp"
#include "SwapChain.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>


namespace ltn {
	class TransformObject : public Component
	{
	public:
		TransformObject (CoreInstance& core_instance , GraphicsPipeline& pipeline);
		~TransformObject();
		void update(FrameUpdateData& framedata) override;
		VkDescriptorSetLayout      get_descriptorset_layout() override;
		void updateUniformBuffer(FrameUpdateData& framedata);
		//auto inline get_descriptorSetLayout() { return m_descriptorSetLayout; }

		std::vector<VkDescriptorSet> m_descriptorSets;
	private:
		void createUniformBuffers();
		CoreInstance& m_core_instance;
		std::vector<VkBuffer> m_uniformBuffers;
		std::vector<VkDeviceMemory> m_uniformBuffersMemory;
		std::vector<void*> m_uniformBuffersMapped;

		VkDescriptorSetLayout m_descriptorSetLayout;
		VkDescriptorPool m_descriptorPool;
		GraphicsPipeline& m_pipeline;

		void createDescriptorSetLayout();
		void createDescriptorPool();
		void createDescriptorSets();

		void cleanup();

	};

}