#pragma once
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>
#include <optional>
#include <cstring>
namespace ltn {
	class CoreInstance
	{
	public:
		CoreInstance(GLFWwindow& window);
		~CoreInstance();

		void create_instance();
		//--------------------
		// Logical device:
		//--------------------	
		void create_device_and_queuefamily();
		void create_surface(GLFWwindow& window);

		//--------------------
		// Physical device:
		//--------------------
		void setup_physical_device();
		void setup_physical_property();
		void setup_physical_features();
		void setup_queuefamily_properties();
		bool is_physical_device_suitable(VkPhysicalDevice device);

		//--------------------
		//  Get / set
		//--------------------
		VkInstance get_instance() { return m_instance; }
		VkSurfaceKHR get_surface() { return m_surface; }
		VkDevice get_device() { return m_device; }
		VkPhysicalDevice get_physical_device() { return m_physicalDevice; }
		inline const auto get_queuefailmy_indexs() const { return &m_queueFamilyIndex; }
		inline const VkQueue graphic_queue() const { return m_graphicsQueue; }
		inline const VkQueue present_queue() const { return m_presentQueue; }
		inline const VkCommandPool& cmd_pool() const { return m_commandPool; }

	private:
		void cleanup();
		// Instance level
		VkInstance m_instance{};
		VkSurfaceKHR m_surface{};

		std::vector<VkExtensionProperties> m_extensions;
		std::vector<const char*> query_instance_extensions();
		void add_validation_layer(VkDeviceCreateInfo& createInfo);
		bool check_validation_layer_valid();
		void setupDebugMessenger();
		// device level
		std::vector<const char*> m_extension_list;
		//--------------------
		// Logical device:
		//--------------------
		bool checkValidationLayerSupport();
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void create_command_pool();
		struct QueueFamilyIndex
		{
			std::optional<uint32_t>                 graphic_queuefamily_index; // graphic command process queue family
			std::optional<uint32_t>                 present_queuefamily_index; // present surface queue family
			bool is_complete() {
				return
					graphic_queuefamily_index.has_value() && present_queuefamily_index.has_value();
			}
		}m_queueFamilyIndex;
		// Device level
		VkDevice m_device{};
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;
		VkCommandPool m_commandPool;

		//--------------------
		// Physical device:
		//--------------------
		//The graphics card that we'll end up selecting will be stored 
		//in a VkPhysicalDevice handle that is added as a new class member.
		//This object will be implicitly destroyed when the VkInstance is destroyed, so we won't need to do anything new in the cleanup function.
		VkPhysicalDevice				m_physicalDevice = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties		m_physical_device_properties{};
		VkPhysicalDeviceFeatures		m_physical_device_features{}; //init by vk_false
		std::vector <VkQueueFamilyProperties> m_physical_queuefamily_properties;
		void get_suitable_queuefamily(VkPhysicalDevice device, QueueFamilyIndex& queue_index);

#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

		//--------------------
		// Other setting :
		//--------------------
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		VkDebugUtilsMessengerEXT m_debugMessenger;

	};

}