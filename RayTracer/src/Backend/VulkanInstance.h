#pragma once

#include "Platform/Window.h"
#include <volk.h>

#include "Common/NonCopyable.h"
#include "Common/NonMovable.h"

#include <string>

namespace VulkanBackend {
    
    /**
     * @class VulkanInstance
     * @brief An RAII class layer over a vulkan instance and debug messenger.
     * 
     * Manages a single Vulkan instance with some required extensions and layers
     * being hard-coded. This class is to be owned by the `VulkanRenderer` class.
     * This class holds a debug messager.
     *
     * This class does NOT manage any devices, but it does provide a facility to
     * choose devices via the `ChoosePhysicalDevice` method.
     * 
     * The extensions we require for this instance are:
     *      - platform-dependent surface extensions
     *      - (MacOS) `VK_KHR_get_physical_device_properties2`
     *      - (MacOS) `VK_KHR_portability_enumeration`
     * We also query for validation and debug extensions and layers if requested.
     *
     * The device extensions we require are:
     *      - (MacOS) `VK_KHR_portability_subset`
     *      - `VK_KHR_swapchain`
     */
    class VulkanInstance : private NonCopyable, private NonMovable {
    public:
    
        /**
         * @brief Vulkan API Version
         * 
         * Currently there is only support for Vulkan version 1.3, including
         * its subset supported by MoltenVk.
         */
        enum class APIVersion {
            version_molten_vk,
            version_1_3,
        };
    
    public:
        /**
         * @param application_name A user-defined application name
         * @param version The vulkan API version required to be supported
         */
        VulkanInstance(const std::string &application_name, APIVersion version, bool enable_validation_layers);
    
        ~VulkanInstance();
    
        /**
         * @brief Chooses a physical device
         * 
         * Currently it just chooses the first available device.
         *
         * TODO: Choose device by a more complicated scheme and ensure requirements are asserted.
         *
         */
        VkPhysicalDevice ChoosePhysicalDevice() const;
    
        /** @brief Creates a VkSurfaceKHR object using GLFW. */
        VkSurfaceKHR CreatePresentSurface(const Platform::Window &window) const;
    
        /** @brief Destroys a VkSurfaceKHR object. */
        void DestroyPresentSurface(VkSurfaceKHR surface) const;
    
        inline VkInstance GetHandle() const { return m_instance; }
    private:
        bool m_validation_layers_enabled = false;
    
        VkInstance m_instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;
    private:
        using ExtensionsAndLayers = std::pair<std::vector<const char *>, std::vector<const char *>>;
        /**
         * @brief Helper function to get the extensions and layers we wish to enable.
         * 
         * @return The tuple (extensions, layers)
         *
         * This method only enforces portability and surface extensions. Other extensions will be
         * queried and determine available features for the VulkanRenderer.
         */
        ExtensionsAndLayers GetExtensionsAndLayers();
    
        /** @brief Creates vulkan instance with required extensions. */
        void CreateInstance(const std::string &application_name, APIVersion version);
        
        /** @brief Debug messenger callback to be used when validation layers are enabled. */
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
            void* user_data);
    
        bool IsDeviceSuitable(VkPhysicalDevice physical_device);
    };

}
