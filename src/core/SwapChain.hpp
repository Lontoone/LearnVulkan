#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
#include <iostream> // for std::out
#include "CoreInstance.hpp"
#include "../utli/helper_functions.hpp"

namespace ltn{
    class SwapChain {
    public:
        SwapChain(CoreInstance& core_instance, int w, int h);
        ~SwapChain(); //todo....

        // With 3 or more frames in flight, the CPU could get ahead of the GPU, adding frames of latency. 
        static const int MAX_FRAMES_IN_FLIGHT = 2;

        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        VkFormat get_format() const { return m_swapChain_image_format; }
        const std::vector<VkImageView>& get_image_views() const { return m_swapChain_image_views; }
        inline const unsigned int _width() const { return m_width; }
        inline const unsigned int _height() const { return m_height; }
        inline const unsigned int current_frame() const { return m_currentFrame; }
        inline const VkSwapchainKHR swap_chain() const { return m_swapchain; }
        inline VkFence& get_fence() { return m_inFlightFences[m_currentFrame]; }  // todo: auto fence pool
        inline const VkSemaphore get_avaliable_semaphore() { return m_imageAvailableSemaphores[m_currentFrame]; }  // todo: auto fence pool
        inline const VkSemaphore get_finish_semaphore() { return m_renderFinishedSemaphores[m_currentFrame]; }  // todo: auto fence pool
        inline const VkImageView& get_depth_img_view() const { return depthImageView; }
        void update_frame_count() { (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT; };

        void cleanup();
    private:
        int m_width;
        int m_height;
        CoreInstance& m_core_instance;
        SwapChainSupportDetails     m_support_details{};
        uint32_t                    m_image_count;
        uint32_t                    m_currentFrame = 0;
        VkSwapchainKHR              m_swapchain;
        //-----------------
        //   Images
        //-----------------
        std::vector<VkImage>        m_swapChain_images;
        VkFormat                    m_swapChain_image_format;
        VkExtent2D                  m_swapChain_extent;
        std::vector<VkImageView>    m_swapChain_image_views;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;          

        // sync object
        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkFence> m_inFlightFences;
        
        // Select supported format
        void query_support();
        VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, const unsigned int w, const unsigned int h);

        void create_images();
        void create_image_view();
        void create_syncobjects();
        void create_depth_buffer();

        void create_swap_chain(const VkSurfaceFormatKHR& format, const VkPresentModeKHR& present_mode, const VkExtent2D& extent);

    };

}