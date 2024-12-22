#include "Model.hpp"


//ltn::Model::Model(VkDevice& _device, VkPhysicalDevice& _physicalDevice) :device{ _device }, physicalDevice{_physicalDevice}
ltn::Model::Model(CoreInstance& core) :coreInstance{core}
{
	createVBO();
}

ltn::Model::~Model()
{
	cleanup();
}

void ltn::Model::bind(VkCommandBuffer& cmdbuffer)
{
	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdbuffer, 0, 1, vertexBuffers, offsets);

}

void ltn::Model::createVBO()
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	/*
	createBuffer(
		coreInstance.get_device(),
		coreInstance.get_physical_device(),
		bufferSize, 
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer, vertexBufferMemory);
	*/
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(
		coreInstance.get_device(),
		coreInstance.get_physical_device(),
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	//Copy to Staging buffer
	void* data;
	vkMapMemory(coreInstance.get_device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(coreInstance.get_device(), stagingBufferMemory);

	// Allocate a memory on GPU that CPU can not access
	createBuffer(
		coreInstance.get_device(),
		coreInstance.get_physical_device(),
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		vertexBuffer,
		vertexBufferMemory);

	copyBuffer(
		coreInstance.get_device(),
		coreInstance.get_queuefailmy_indexs()->graphic_queuefamily_index.value(),
		coreInstance.graphic_queue(),
		stagingBuffer, 
		vertexBuffer, 
		bufferSize);

	vkDestroyBuffer(coreInstance.get_device(), stagingBuffer, nullptr);
	vkFreeMemory(coreInstance.get_device(), stagingBufferMemory, nullptr);


}

void ltn::Model::cleanup()
{
	vkDestroyBuffer(coreInstance.get_device(), vertexBuffer, nullptr);
	vkFreeMemory(coreInstance.get_device(), vertexBufferMemory, nullptr);
}
