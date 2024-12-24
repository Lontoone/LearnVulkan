#include "GameObject.hpp"

void ltn::GameObject::add_component(Component* comp)
{
	this->m_components.emplace_back(comp);
}

std::vector<VkDescriptorSetLayout>* ltn::GameObject::get_all_descriptorLayouts()
{
	for (int i = 0; i < m_components.size(); ++i) {
		auto descrip = m_components[i]->get_descriptorset_layout();
		if (descrip != NULL) {
			m_descriptorSetLayouts.push_back(descrip);
		}
	}

	return &m_descriptorSetLayouts;
}

void ltn::GameObject::execute(FrameUpdateData& frame_data)
{
	for (int i = 0; i < m_components.size(); ++i) {
		m_components[i]->update(frame_data);
	}
}

