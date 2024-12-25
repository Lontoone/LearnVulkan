#include "GameObject.hpp"

void ltn::GameObject::add_component(Component* comp)
{
	comp->index = this->m_components.size();
	this->m_components.emplace_back(comp);
}

void ltn::GameObject::remove_component(int index)
{
	this->m_components.erase(this->m_components.begin() + index);
	// update index
	int i = 0;
	for (auto cmp : this->m_components) {
		cmp->index = i;
		++i;
	}
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

