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
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(vertices[0]) * vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	// The buffer will only be used from the graphics queue, 
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateBuffer(coreInstance.get_device(), &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(coreInstance.get_device(), vertexBuffer, &memRequirements);
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(coreInstance.get_physical_device(), memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if (vkAllocateMemory(coreInstance.get_device(), &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}
	vkBindBufferMemory(coreInstance.get_device(), vertexBuffer, vertexBufferMemory, 0);

	//copy data
	void* data;
	vkMapMemory(coreInstance.get_device(), vertexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(coreInstance.get_device(), vertexBufferMemory);
}

void ltn::Model::cleanup()
{
	vkDestroyBuffer(coreInstance.get_device(), vertexBuffer, nullptr);
	vkFreeMemory(coreInstance.get_device(), vertexBufferMemory, nullptr);
}
