#pragma once

#include "CoreInstance.hpp"

#include <LearnVulkan/src/utli/helper_functions.hpp>

namespace ltn{
    class Image {

    public:
        Image(CoreInstance& core_instance);
        ~Image();

        void load_texture(const char* path);
        inline VkDescriptorSetLayout& get_descriptorsetLayout(){return m_descriptorSetLayout;}
        inline VkDescriptorSet& get_descriptorset() { return m_descriptorSets; }
    private:
        int m_width, m_height, m_channel;
        CoreInstance& m_core_instance;

        VkBuffer m_staging_buffer;
        VkDeviceMemory m_staging_buffer_memory;

        VkImage m_texture_image;
        VkDeviceMemory m_texture_image_memory;
        VkImageView m_texture_imageView;
        VkSampler m_texture_sampler;

        VkDescriptorSet m_descriptorSets;
        VkDescriptorPool m_descriptorPool;
        VkDescriptorSetLayout m_descriptorSetLayout;

        void create_texture();
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void createTextureImageView();
        void createTextureSampler();

        void createDescriptorSetLayout();
        void create_descriptor_pool();
        void create_descriptor();

        // helper function
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void cleanup();
    };
}