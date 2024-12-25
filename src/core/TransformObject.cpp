#include "TransformObject.hpp"


ltn::TransformObject::TransformObject(CoreInstance& core_instance, GraphicsPipeline& pipeline) 
	: m_core_instance{ core_instance }, m_pipeline{pipeline}
{
	createUniformBuffers();
	createDescriptorSetLayout();
	createDescriptorPool();
	createDescriptorSets();
}

ltn::TransformObject::~TransformObject()
{
    cleanup();
}

void ltn::TransformObject::update(FrameUpdateData& framedata) 
{
    updateUniformBuffer(framedata);
}

VkDescriptorSetLayout ltn::TransformObject::get_descriptorset_layout()
{
	return m_descriptorSetLayout;
}

void ltn::TransformObject::updateUniformBuffer(FrameUpdateData& framedata)
{
    // dummy test
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), framedata.aspect_ratio , 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    memcpy(m_uniformBuffersMapped[framedata.current_image], &ubo, sizeof(ubo));

	// update desc set
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = m_uniformBuffers[framedata.current_image];
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = m_descriptorSets[framedata.current_image];
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;
	descriptorWrite.pImageInfo = nullptr; // Optional
	descriptorWrite.pTexelBufferView = nullptr; // Optional
	vkUpdateDescriptorSets(m_core_instance.get_device(), 1, &descriptorWrite, 0, nullptr);

	vkCmdBindDescriptorSets(
		framedata.cmdbuf,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		m_pipeline.get_layout(),
		0,
		1,
		&m_descriptorSets[framedata.current_image], 0, nullptr);

}

void ltn::TransformObject::createUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(
            m_core_instance.get_device(),
            m_core_instance.get_physical_device(),
            bufferSize, 
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            m_uniformBuffers[i],
            m_uniformBuffersMemory[i]);
        // persistent mapping
        vkMapMemory(m_core_instance.get_device(), m_uniformBuffersMemory[i], 0, bufferSize, 0, &m_uniformBuffersMapped[i]);
    }
}


void ltn::TransformObject::createDescriptorSetLayout()
{
	// MVP uniform data
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1; //Change this if you have multiple objects.


	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional // only relevant for image sampling related descriptors

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(m_core_instance.get_device(), &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void ltn::TransformObject::createDescriptorPool()
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(m_core_instance.get_device(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void ltn::TransformObject::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();
	m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(m_core_instance.get_device(), &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
}

void ltn::TransformObject::cleanup()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(m_core_instance.get_device(), m_uniformBuffers[i], nullptr);
        vkUnmapMemory(m_core_instance.get_device(), m_uniformBuffersMemory[i]);
        vkFreeMemory(m_core_instance.get_device(), m_uniformBuffersMemory[i], nullptr);
    }

	vkDestroyDescriptorPool(this->m_core_instance.get_device(), m_descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(this->m_core_instance.get_device(), m_descriptorSetLayout, nullptr);

    //vkDestroyDescriptorSetLayout((m_core_instance.get_device(), descriptorSetLayout, nullptr);

}
