#include <utility>

#include "VulkanInstance.h"
#include "debug.h"

#include <format>
#include <cassert>
#include <iostream>
#include <vector>

namespace VulkanBackend {
    
    VulkanInstance::VulkanInstance(const std::string &application_name, APIVersion version, bool enable_validation_layers)
        : m_validation_layers_enabled(enable_validation_layers)
    {
        CreateInstance(application_name, version);
        volkLoadInstance(m_instance);
    }
    
    VulkanInstance::~VulkanInstance() {
        std::cout << "  Destroying Vulkan Instance" << std::endl;
    
        vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }
    
    VkPhysicalDevice VulkanInstance::ChoosePhysicalDevice() const {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
        assert(device_count > 0);
    
        std::vector<VkPhysicalDevice> physical_devices(device_count);
        VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &device_count, physical_devices.data()));
    
        // TODO: Update physical devices so that we choose based on a complex criteria and that we
        // TODO: actually have required extensions.
    
        {
            VkPhysicalDeviceProperties device_properties;
            vkGetPhysicalDeviceProperties(physical_devices[0], &device_properties);
            std::cout << "  Chose physical device: " << device_properties.deviceName << std::endl;
        }
        return physical_devices[0];
    }
    
    VkSurfaceKHR VulkanInstance::CreatePresentSurface(const Platform::Window &window) const {
        std::cout << "  Creating Surface" << std::endl;
    
        VkSurfaceKHR surface;
        glfwCreateWindowSurface(m_instance, window.GetHandle(), nullptr, &surface);
        return surface;
    }
    
    void VulkanInstance::DestroyPresentSurface(VkSurfaceKHR surface) const {
        vkDestroySurfaceKHR(m_instance, surface, nullptr);
    }
    
    void VulkanInstance::CreateInstance(const std::string &application_name, APIVersion version) {
        std::cout << "  Creating Vulkan Instance" << std::endl;
    
        VkApplicationInfo application_info {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = application_name.c_str(),
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "No Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        };
    
        // Choose API version
        switch (version) {
            case APIVersion::version_molten_vk:
            case APIVersion::version_1_3:
                application_info.apiVersion = VK_API_VERSION_1_3;
                break;
        }
    
        // Grab needed extensions and layers and prepare instance info
        auto [enabled_extensions, enabled_layers] = GetExtensionsAndLayers();
    
        VkInstanceCreateInfo instance_create_info {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &application_info,
            .enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size()),
            .ppEnabledExtensionNames = enabled_extensions.data(),
            .enabledLayerCount = static_cast<uint32_t>(enabled_layers.size()),
            .ppEnabledLayerNames = enabled_layers.data(),
        };
    
        VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info {};
    
        if (m_validation_layers_enabled) {
            // Set up debug messenger
    
            debug_messenger_create_info = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .pNext = nullptr,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = VulkanInstance::DebugCallback,
                .pUserData = nullptr,
            };
    
            instance_create_info.pNext = &debug_messenger_create_info;
        }
    
    #ifdef __APPLE__
        // MoltenVk requires portability flag set.
        instance_create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    #endif
    
        std::cout << "    Enabling extensions: " << std::endl;
        for (const auto &extension_name : enabled_extensions) {
            std::cout << "     - " << extension_name << std::endl;
        }
        std::cout << "    Enabling layers: " << std::endl;
        for (const auto &layer_name : enabled_layers) {
            std::cout << "     - " << layer_name << std::endl;
        }
    
        VK_CHECK(vkCreateInstance(&instance_create_info, nullptr, &m_instance));
    
        volkLoadInstance(m_instance);
    
        if (m_validation_layers_enabled) {
            std::cout << "  Creating Debug Messenger" << std::endl;
            VK_CHECK(vkCreateDebugUtilsMessengerEXT(m_instance, &debug_messenger_create_info, nullptr, &m_debug_messenger));
        }
    }
    
    VulkanInstance::ExtensionsAndLayers VulkanInstance::GetExtensionsAndLayers() {
        // Get all supported extensions
        uint32_t extension_count = 0;
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
        assert(extension_count > 0);
    
        std::vector<VkExtensionProperties> supported_extensions(extension_count);
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, supported_extensions.data()));
    
        // Get all supported layers
        uint32_t layer_count = 0;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
        assert(layer_count > 0);
    
        std::vector<VkLayerProperties> supported_layers(layer_count);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&layer_count, supported_layers.data()));
    
        /**
         * @brief Checks if extension with name `extension_name` is supported by this instance.
         * 
         * @param extension_name
         */
        const auto extension_is_supported = [&](const char *extension_name) {
            for (auto extension : supported_extensions) {
                if (std::strcmp(extension.extensionName, extension_name) == 0)
                    return true;
            }
            return false;
        };
    
        std::vector<const char *> extensions {};
    
    #ifdef __APPLE__
        // Push Apple portability extensions
    
        assert(extension_is_supported(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME));
        extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    
        assert(extension_is_supported(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME));
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    #endif
        
        {
            // Push GLFW surface extensions
            uint32_t count;
            const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&count);
    
            for (int i = 0; i < count; ++i) {
                assert(extension_is_supported(glfw_extensions[i]));
                extensions.push_back(glfw_extensions[i]);
            }
        }
    
        // Push debug extensions
    
        std::vector<const char *> layers {};
    
        if (m_validation_layers_enabled) {
            if (extension_is_supported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                layers.push_back("VK_LAYER_KHRONOS_validation");
            } else {
                std::cout << "Warning: Validation layers are enabled, but we cannot load the debug extension or validation layers! Turning off validation." << std::endl;
                m_validation_layers_enabled = false;
            }
        }
    
        return std::make_pair(extensions, layers);
    }
    
    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
            void* user_data)
    {
        std::string prefix;
        if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
            prefix = "VERBOSE";
        else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
            prefix = "INFO";
        else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            prefix = "WARNING";
        else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            prefix = "ERROR";
    
        fprintf(stderr, "Validation Message [%s]: {%s}", prefix.c_str(), callback_data->pMessage);
    
        return VK_FALSE;
    }

}
