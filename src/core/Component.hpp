#pragma once
#include <cstddef>
#include <vulkan/vulkan.h>
#include "../utli/structure.hpp"
namespace ltn{
    class Component
    {
    public:
        virtual VkDescriptorSetLayout get_descriptorset_layout() { return NULL; };
        //virtual void update(FrameUpdateData& updateData ) {  };
        //virtual void bind(const VkCommandBuffer& cmdBuf) {};        
        virtual void update(FrameUpdateData& framedata) {};
    private:
        
    };
}