#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Image.hpp"
#include "config.hpp"
ltn::Image::Image(CoreInstance& core_instance) :m_core_instance{ core_instance }
{
}

ltn::Image::~Image()
{
    cleanup();
}

void ltn::Image::load_texture(const char* path)
{
    // STBI_rgb_alpha value forces the image to be loaded with an alpha channel, even if it doesn't have one.
    // The pixels are laid out row by row with 4 bytes per pixel in the case of STBI_rgb_alpha.
    stbi_uc* pixels = stbi_load(path, &m_width, &m_height, &m_channel, STBI_rgb_alpha);
    mipLevels = static_cast<uint32_t>(::floor(::log2(std::max(m_width, m_height)))) + 1;
    VkDeviceSize imageSize = m_width * m_height * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }
    // create a buffer to hold the pixel data
    createBuffer(
        m_core_instance.get_device(),
        m_core_instance.get_physical_device(),
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        m_staging_buffer,
        m_staging_buffer_memory);

    void* data;
    vkMapMemory(m_core_instance.get_device(), m_staging_buffer_memory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(m_core_instance.get_device(), m_staging_buffer_memory);
    stbi_image_free(pixels);
    // move buffer to image memory
    create_texture();
    createTextureImageView();
    createTextureSampler();
    generateMipmaps();

    //----------------
    //  Descriptor
    //----------------
    create_descriptor_pool();
    createDescriptorSetLayout();
    create_descriptor();
}

void ltn::Image::create_texture()
{
    //---------------
    //      Create Image Object
    //---------------
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(m_width);
    imageInfo.extent.height = static_cast<uint32_t>(m_height);
    imageInfo.extent.depth = 1; // 2D image = 1
    //imageInfo.mipLevels = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;  // should be the same as in swap chain
    // If you want to be able to directly access texels in the memory of the image, 
    // then you must use VK_IMAGE_TILING_LINEAR. 
    //      *VK_IMAGE_TILING_LINEAR: Texels are laid out in row-major order like our pixels array
    //      *VK_IMAGE_TILING_OPTIMAL: Texels are laid out in an implementation defined order for optimal access
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL; // the tiling mode cannot be changed 
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // Samples flag is related to multisampling. This is only relevant for images 
    // that will be used as attachments, so stick to one sample.  (Useful in stoeing 3D terrain map)
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional

    if (vkCreateImage(m_core_instance.get_device(), &imageInfo, nullptr, &m_texture_image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    //---------------
    //      Bind image to memory
    //---------------
    VkMemoryRequirements memRequirements;
    //vkGetBufferMemoryRequirements
    vkGetImageMemoryRequirements(m_core_instance.get_device(), m_texture_image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(m_core_instance.get_physical_device(), memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(m_core_instance.get_device(), &allocInfo, nullptr, &m_texture_image_memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }
    //vkBindBufferMemory.
    vkBindImageMemory(m_core_instance.get_device(), m_texture_image, m_texture_image_memory, 0);

    //---------------
   //    Move from staging buffer to texture image
   //---------------
    transitionImageLayout(
        m_texture_image,
        VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(
        m_staging_buffer, m_texture_image,
        static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));

    // For shader to access it
    transitionImageLayout(
        m_texture_image,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void ltn::Image::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(
        m_core_instance.get_device(),
        m_core_instance.cmd_pool());

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    // One of the most common ways to perform layout transitions is using an image memory barrier.
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout; //use VK_IMAGE_LAYOUT_UNDEFINED as oldLayout if you don't care about the existing contents
    barrier.newLayout = newLayout;
    // If you are using the barrier to transfer queue family ownership, then 
    // these two fields should be the indices of the queue families.They must
    // be set to VK_QUEUE_FAMILY_IGNORED if you don't want to do this (not 
    // the default value!).
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    //  Our image is not an array and does not have mipmapping levels
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    // Case : Undefined ¡÷ transfer destination
    //      transfer writes that don't need to wait on anything
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;  //earliest possible pipeline stage
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT; //is not a real stage within the graphics and compute pipelines. 
    }
    // Case : Transfer destination ¡÷ shader reading
    //      shader reads should wait on transfer writes, specifically the shader
    //      reads in the fragment shader, because that's where we're going to use the texture
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }
    // operations that involve the resource must happen before the barrier
    //barrier.srcAccessMask = 0; // TODO
    // operations that involve the resource must wait on the barrier
    //barrier.dstAccessMask = 0; // TODO
    vkCmdPipelineBarrier(
        commandBuffer,
        // in which pipeline stage the operations occur that should happen before the barrier.
        sourceStage,
        // the pipeline stage in which operations will wait on the barrier.
        destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );  // Shader stage only : https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap7.html#synchronization-access-types-supported


    endSingleTimeCommands(m_core_instance, commandBuffer);
}

void ltn::Image::createTextureImageView()
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_texture_image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(
        m_core_instance.get_device(),
        &viewInfo, nullptr, &m_texture_imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
}

void ltn::Image::createTextureSampler()
{
    // Get maxAnisotropy
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_core_instance.get_physical_device(), &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    //samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0;  // The base resolution when close to camera
    //samplerInfo.minLod = static_cast<float>(mipLevels / 2);  // For debug
    samplerInfo.maxLod = static_cast<float>(mipLevels);

    samplerInfo.anisotropyEnable = VK_FALSE;  // Temp
    samplerInfo.maxAnisotropy = 1.0f;
    if (vkCreateSampler(m_core_instance.get_device(), &samplerInfo, nullptr, &m_texture_sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void ltn::Image::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 0;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; // for shader to access
    //samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::vector<VkDescriptorSetLayoutBinding> bindings = { samplerLayoutBinding };


    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(
        m_core_instance.get_device()
        , &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(1);
    allocInfo.pSetLayouts = &m_descriptorSetLayout;

    if (vkAllocateDescriptorSets(
        m_core_instance.get_device(),
        &allocInfo,
        &m_descriptorSets) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}

void ltn::Image::create_descriptor_pool()
{
    // Create pool helps to catch the VK_ERROR_POOL_OUT_OF_MEMORY error:
    VkDescriptorPoolSize poolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER , 1 };
    std::vector<VkDescriptorPoolSize > poolSizes = { poolSizes };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    //poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(
        m_core_instance.get_device(),
        &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void ltn::Image::create_descriptor()
{
    std::vector<VkWriteDescriptorSet> descriptorWrites{};

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = m_texture_imageView;
    imageInfo.sampler = m_texture_sampler;

    VkWriteDescriptorSet set{};
    set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    set.dstSet = m_descriptorSets;
    set.dstBinding = 0;
    set.dstArrayElement = 0;
    set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    set.descriptorCount = 1;
    set.pImageInfo = &imageInfo;

    descriptorWrites.push_back(set);

    vkUpdateDescriptorSets(
        m_core_instance.get_device(),
        static_cast<uint32_t>(descriptorWrites.size()),
        descriptorWrites.data(), 0, nullptr);

}

void ltn::Image::generateMipmaps()
{
    // vkCmdBlitImage require support Linear filtering
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_core_instance.get_physical_device(),
        VK_FORMAT_R8G8B8A8_SRGB,  // hard-coded imageFormat
        &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    VkCommandBuffer commandBuffer = beginSingleTimeCommands(m_core_instance.get_device() , m_core_instance.cmd_pool());

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = m_texture_image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;
       
    int32_t mipWidth = m_width;
    int32_t mipHeight = m_height;
    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1; //wait for previous level to be done
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;
        vkCmdBlitImage(commandBuffer,
            m_texture_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            m_texture_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);
    endSingleTimeCommands(m_core_instance,commandBuffer);
}

void ltn::Image::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(
        m_core_instance.get_device(),
        m_core_instance.cmd_pool());

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    // 0 indicates that the pixels are simply tightly packed
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    // Which part of the image (crop)
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };
    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
    endSingleTimeCommands(m_core_instance, commandBuffer);
}

void ltn::Image::cleanup()
{
    transitionImageLayout(m_texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(m_core_instance.get_device(), m_staging_buffer, nullptr);
    vkFreeMemory(m_core_instance.get_device(), m_staging_buffer_memory, nullptr);
    vkDestroyImageView(m_core_instance.get_device(), m_texture_imageView, nullptr);

    vkDestroyImage(m_core_instance.get_device(), m_texture_image, nullptr);
    vkFreeMemory(m_core_instance.get_device(), m_texture_image_memory, nullptr);
    vkDestroySampler(m_core_instance.get_device(), m_texture_sampler, nullptr);
}
