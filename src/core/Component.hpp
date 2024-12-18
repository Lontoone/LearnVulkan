#pragma once
#include <cstddef>
#include <vulkan/vulkan.h>

namespace ltn{
    class Component
    {
    public:
        virtual VkDescriptorSetLayout get_descriptorset_layout() { return NULL; };
        //virtual void update(FrameUpdateData& updateData ) {  };
        //virtual void bind(const VkCommandBuffer& cmdBuf) {};
        
    private:
        
    };
}