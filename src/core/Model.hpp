#pragma once
#include <glm/glm.hpp>
#include "../utli/structure.hpp"
#include "vulkan/vulkan.h"
#include "../utli//helper_functions.hpp"
#include "../core/CoreInstance.hpp"
#include <vector>
#include <stdexcept>
namespace ltn{
    class Model
    {    
    public:
        Model(CoreInstance& core);
        ~Model();

        void bind(VkCommandBuffer& cmdbuffer);
    private:
        // temp dummy data:
        const std::vector<Vertex> vertices = {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };

        CoreInstance& coreInstance;
        void createVBO();
        void cleanup();

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
    };
    
    
}