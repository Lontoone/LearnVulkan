#pragma once
#include <glm/glm.hpp>
#include "../utli/structure.hpp"
#include "vulkan/vulkan.h"
#include "../utli//helper_functions.hpp"
#include "../core/Component.hpp"
#include "../core/CoreInstance.hpp"
#include "GraphicsPipeline.hpp"
#include <vector>
#include <stdexcept>
namespace ltn{
    class Model : public Component
    {    
    public:
        Model(CoreInstance& core, GraphicsPipeline& pipeline);
        ~Model();

       //-----------------
       //  Component class 
       //-----------------
        void            update(FrameUpdateData& update_data) override;

        void bind(VkCommandBuffer& cmdbuffer);
        void draw(VkCommandBuffer& cmdbuffer);
    private:
        // temp dummy data:
        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
        };
        const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
        };

        CoreInstance& coreInstance;
        GraphicsPipeline& m_pipeline;
        void createVertexBuffer();
        void createIndexBuffer();
        void cleanup();

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;

    };
    
    
}