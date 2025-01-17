#pragma once

#include "./Component.hpp"
#include <vector>
#include <vulkan/vulkan.h>
#include "../utli/structure.hpp"
namespace ltn{

class GameObject
{
public:
	std::vector<Component*> m_components;
	void add_component(Component* comp);
	void remove_component(int index);
	std::vector<VkDescriptorSetLayout>* get_all_descriptorLayouts();
	std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;

	//void update_descriptor();
	//void bind(const VkCommandBuffer& cmdBuf);
	void execute(FrameUpdateData& frame_data);
	void execute_before_frame(FrameUpdateData& frame_data);
	void execute_after_frame(FrameUpdateData& frame_data);
private:

};

}