#include "Model.hpp"


//ltn::Model::Model(VkDevice& _device, VkPhysicalDevice& _physicalDevice) :device{ _device }, physicalDevice{_physicalDevice}
ltn::Model::Model(CoreInstance& core) :coreInstance{core}
{
	createVertexBuffer();
	createIndexBuffer();
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
	vkCmdBindIndexBuffer(cmdbuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
}

void ltn::Model::draw(VkCommandBuffer& cmdbuffer)
{
	vkCmdDrawIndexed(cmdbuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

void ltn::Model::createVertexBuffer()
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

void ltn::Model::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(coreInstance.get_device(),coreInstance.get_physical_device(),
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(coreInstance.get_device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(coreInstance.get_device(), stagingBufferMemory);

	//GPU local buffer
	createBuffer(coreInstance.get_device(), coreInstance.get_physical_device(),
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	//Copy from staging buffer
	copyBuffer(
		coreInstance.get_device(),
		coreInstance.get_queuefailmy_indexs()->graphic_queuefamily_index.value(),
		coreInstance.graphic_queue(),stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(coreInstance.get_device(), stagingBuffer, nullptr);
	vkFreeMemory(coreInstance.get_device(), stagingBufferMemory, nullptr);
}

void ltn::Model::cleanup()
{
	// vertex buffer
	vkDestroyBuffer(coreInstance.get_device(), vertexBuffer, nullptr);
	vkFreeMemory(coreInstance.get_device(), vertexBufferMemory, nullptr);
	// index buffer
	vkDestroyBuffer(coreInstance.get_device(), vertexBuffer, nullptr);
	vkFreeMemory(coreInstance.get_device(), vertexBufferMemory, nullptr);
}
