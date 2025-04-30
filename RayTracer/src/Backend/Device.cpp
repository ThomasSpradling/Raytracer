#include "Device.h"
#include "debug.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace VulkanBackend {
    
    Device::Device(const VulkanInstance &instance, VkSurfaceKHR surface)
        : m_instance(instance)
    {
        std::cout << "  Creating Logical Device" << std::endl;
    
        m_physical_device = instance.ChoosePhysicalDevice();
    
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queue_family_count, nullptr);
        assert(queue_family_count > 0);
    
        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queue_family_count, queue_families.data());
    
        std::optional<uint32_t> graphics_candidate;
        std::optional<uint32_t> present_candidate;
        std::optional<uint32_t> compute_candidate;
        std::optional<uint32_t> transfer_candidate;
    
        // TODO: Make sure compute and transfer queue are also distinct from eachother if
        // TODO: possible. In addition, do better error checking for not having a queue.
    
        for (uint32_t i = 0; i < queue_family_count; ++i) {
            const auto &queue_family = queue_families[i];
    
            if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                if (!graphics_candidate.has_value())
                    graphics_candidate = i;
            }
    
            VkBool32 present_support = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device, i, surface, &present_support);
            if (present_support == VK_TRUE) {
                if (!present_candidate.has_value())
                    present_candidate = i;
            }
    
            if (queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                if (!(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) && !compute_candidate.has_value())
                    compute_candidate = i;
            }
    
            if (queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
                if (!(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) && !transfer_candidate.has_value()) {
                    transfer_candidate = i;
                }
            }
        }
    
        if (!compute_candidate.has_value() && graphics_candidate.has_value())
            compute_candidate = graphics_candidate;
        if (!transfer_candidate.has_value() && graphics_candidate.has_value())
            transfer_candidate = graphics_candidate;
    
        assert(queue_families[graphics_candidate.value()].queueFlags & VK_QUEUE_GRAPHICS_BIT);
        assert(queue_families[compute_candidate.value()].queueFlags & VK_QUEUE_COMPUTE_BIT);
        assert(queue_families[transfer_candidate.value()].queueFlags & VK_QUEUE_TRANSFER_BIT);
    
        VkBool32 present_support;
        vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device, present_candidate.value(), surface, &present_support);
        assert(present_support);
        
        if (graphics_candidate.has_value())
            m_graphics_queue = Queue{ .family_index = graphics_candidate.value() };
        if (present_candidate.has_value())
            m_present_queue = Queue{ .family_index = present_candidate.value() };
        if (compute_candidate.has_value())
            m_compute_queue = Queue{ .family_index = compute_candidate.value() };
        if (transfer_candidate.has_value())
            m_transfer_queue = Queue{ .family_index = transfer_candidate.value() };
        
        std::set<uint32_t> unique_family_indices {
            m_graphics_queue->family_index,
            m_present_queue->family_index,
            m_compute_queue->family_index,
            m_transfer_queue->family_index
        };
    
        std::cout << "    Chose Device Queue Families: " << std::endl;
        std::cout << "     - [graphics]: " << m_graphics_queue->family_index << std::endl;
        std::cout << "     - [present]: " << m_present_queue->family_index << std::endl;
        std::cout << "     - [compute]: " << m_compute_queue->family_index << std::endl;
        std::cout << "     - [transfer]: " << m_transfer_queue->family_index << std::endl;
    
        std::vector<VkDeviceQueueCreateInfo> queues {};
    
        float queue_priority = 1.0f;
        for (uint32_t family : unique_family_indices) {
            VkDeviceQueueCreateInfo queue_create_info {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = family,
                .queueCount = 1,
                .pQueuePriorities = &queue_priority,
            };
            
            queues.push_back(queue_create_info);
        }
    
        uint32_t device_extension_count = 0;
        vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &device_extension_count, nullptr);
        std::vector<VkExtensionProperties> supported_extensions(device_extension_count);
        vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &device_extension_count, supported_extensions.data());
    
        std::vector<const char *> enabled_extensions {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    #ifdef __APPLE__
            "VK_KHR_portability_subset",
    
            // TODO: These need to be enabled if instance has API version < 1.3. Check for this
            // TODO: rather than hardcoding APPLE
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
            "VK_KHR_copy_commands2",
    #endif
        };
    
        std::cout << "    Enabling device extensions:\n";
        for (const auto &extension : enabled_extensions) {
            bool found = false;
            for (const auto &ext : supported_extensions) {
                if (strcmp(ext.extensionName, extension) == 0) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                std::cerr << "Required device extension not supported: " << extension << std::endl;
                assert(false && "Missing required device extension");
            } else {
                std::cout << "     - " << extension << std::endl;
            }
        }
        
        // TODO: Add these as features in the case we actually do support Vulkan13
    
    #ifdef __APPLE__
        VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
            .pNext = nullptr,
            .dynamicRendering = VK_TRUE
        };
    
        VkPhysicalDeviceSynchronization2FeaturesKHR sync_2_features {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR,
            .pNext = &dynamic_rendering_features,
            .synchronization2 = VK_TRUE
        };
    #else
        VkPhysicalDeviceVulkan13Features vulkan13_features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
            .pNext = nullptr,
            .dynamicRendering = VK_TRUE,
            .synchronization2 = VK_TRUE
        };
    #endif
    
        VkPhysicalDeviceVulkan12Features vulkan12_features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
            .pNext = nullptr,
            .bufferDeviceAddress = VK_TRUE,
            .descriptorIndexing = VK_TRUE
        };
    
    #ifndef __APPLE__
        vulkan12_features.pNext = &vulkan13_features;
    #else
        vulkan12_features.pNext = &sync_2_features;
    #endif
    
        VkDeviceCreateInfo device_create_info {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &vulkan12_features,
            .queueCreateInfoCount = static_cast<uint32_t>(queues.size()),
            .pQueueCreateInfos = queues.data(),
            .enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size()),
            .ppEnabledExtensionNames = enabled_extensions.data(),
        };
    
        VK_CHECK(vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device));
    
        if (m_graphics_queue.has_value())
            vkGetDeviceQueue(m_device, m_graphics_queue->family_index, 0, &m_graphics_queue->queue);
        if (m_present_queue.has_value())
            vkGetDeviceQueue(m_device, m_present_queue->family_index, 0, &m_present_queue->queue);
        if (m_compute_queue.has_value())
            vkGetDeviceQueue(m_device, m_compute_queue->family_index, 0, &m_compute_queue->queue);
        if (m_transfer_queue.has_value())
            vkGetDeviceQueue(m_device, m_transfer_queue->family_index, 0, &m_transfer_queue->queue);
    
        volkLoadDevice(m_device);
    
        // Memory allocation
    
        VmaVulkanFunctions vulkan_functions = {
            .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
            .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
        };
    
        VmaAllocatorCreateInfo allocatorInfo = {
            .physicalDevice = m_physical_device,
            .device = m_device,
            .instance = m_instance.GetHandle(),
            .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
            .pVulkanFunctions = &vulkan_functions,
        };
        vmaCreateAllocator(&allocatorInfo, &m_allocator);
    }
    
    Device::~Device() {
        std::cout << "  Destroying device" << std::endl;
        
        vmaDestroyAllocator(m_allocator);
        vkDestroyDevice(m_device, nullptr);
    }
    
    Device::Queue Device::_GetQueue(QueueType queue) const {
        switch (queue) {
            case QueueType::graphics: {
                assert(m_graphics_queue.has_value());
                return m_graphics_queue.value();
            }
            case QueueType::present: {
                assert(m_present_queue.has_value());
                return m_present_queue.value();
            }
            case QueueType::compute: {
                assert(m_compute_queue.has_value());
                return m_compute_queue.value();
            }
            case QueueType::transfer: {
                assert(m_transfer_queue.has_value());
                return m_transfer_queue.value();
            }
        }
    }
    
    VkQueue Device::GetQueue(QueueType queue) const {
        return _GetQueue(queue).queue;
    }
    
    uint32_t Device::GetQueueIndex(QueueType queue) const {
        return _GetQueue(queue).family_index;
    }
    
    VkSemaphore Device::CreateSemaphore() {
        VkSemaphoreCreateInfo semaphore_create_info {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
        };
    
        VkSemaphore semaphore;
        VK_CHECK(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &semaphore));
        return semaphore;
    }
    
    void Device::DestroySemaphore(VkSemaphore semaphore) {
        vkDestroySemaphore(m_device, semaphore, nullptr);
    }
    
    VkFence Device::CreateFence(bool signalled) {
        VkFenceCreateInfo fence_create_info {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
        };
    
        if (signalled) {
            fence_create_info.flags |= VK_FENCE_CREATE_SIGNALED_BIT;
        }
    
        VkFence fence;
        VK_CHECK(vkCreateFence(m_device, &fence_create_info, nullptr, &fence));
        return fence;
    }
    
    void Device::DestroyFence(VkFence fence) {
        vkDestroyFence(m_device, fence, nullptr);
    }
    
    void Device::WaitForFence(VkFence fence, uint64_t timeout) {
        WaitForAllFences({ fence }, timeout);
    }
    
    void Device::WaitForAllFences(const std::vector<VkFence> &fences, uint64_t timeout) {
        VkResult res = vkWaitForFences(
            m_device,
            static_cast<uint32_t>(fences.size()),
            fences.data(),
            VK_TRUE,
            timeout
        );
    
        if (res == VK_TIMEOUT) {
            throw std::runtime_error("Fence timed out!");
        }
    
        VK_CHECK(res);
    }
    
    void Device::WaitForOneFence(const std::vector<VkFence> &fences, uint64_t timeout) {
        VK_CHECK(vkWaitForFences(
            m_device,
            static_cast<uint32_t>(fences.size()),
            fences.data(),
            VK_FALSE,
            DEFAULT_FENCE_TIMEOUT_TIME
        ));
    }
    
    void Device::ResetFence(VkFence fence) {
        ResetFences({ fence });
    }
    
    void Device::ResetFences(const std::vector<VkFence> &fences) {
        VK_CHECK(vkResetFences(
            m_device,
            static_cast<uint32_t>(fences.size()),
            fences.data()
        ));
    }
    
    // Helper for use in allocating buffers and images: Sets the sharing mode-related properties of the passed
    // in info, given shared_queues
    template <typename T>
    void _SetSharingMode(T &create_info, const std::vector<Device::QueueType> &shared_queues, const Device *device) {
        if (!shared_queues.empty()) {
            std::set<uint32_t> unique_queue_families;
            for (auto queue : shared_queues) {
                unique_queue_families.insert(device->GetQueueIndex(queue));
            }
    
            std::vector<uint32_t> unique_queue_family_vector(unique_queue_families.begin(), unique_queue_families.end());
            if (unique_queue_family_vector.size() >= 2) {
                create_info.queueFamilyIndexCount = static_cast<uint32_t>(unique_queue_family_vector.size());
                create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
                create_info.pQueueFamilyIndices = unique_queue_family_vector.data();
            }
        }
    }
    
    std::pair<VkBuffer, VmaAllocation> Device::AllocateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, std::vector<QueueType> shared_queues, VkMemoryPropertyFlags memory_flags, VmaAllocationCreateFlags allocation_flags) {        
        VkBufferCreateInfo buffer_create_info {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .size = size,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
        };
    
        _SetSharingMode(buffer_create_info, shared_queues, this);
    
        if (!shared_queues.empty()) {
            std::set<uint32_t> shared_queue_families;
            for (auto queue : shared_queues) {
                shared_queue_families.insert(GetQueueIndex(queue));
            }
        
            std::vector unique_queue_families = std::vector(shared_queue_families.begin(), shared_queue_families.end());
            if (unique_queue_families.size() >= 2) {
                buffer_create_info.queueFamilyIndexCount = unique_queue_families.size();
                buffer_create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
                buffer_create_info.pQueueFamilyIndices = unique_queue_families.data();
            }
        }
    
        VmaAllocationCreateInfo allocation_create_info {
            .usage = VMA_MEMORY_USAGE_AUTO,
            .requiredFlags = memory_flags,
            .flags = allocation_flags,
        };
    
        VkBuffer buffer;
        VmaAllocation allocation;
        VK_CHECK(vmaCreateBuffer(m_allocator, &buffer_create_info, &allocation_create_info, &buffer, &allocation, nullptr));
    
        return std::make_pair(buffer, allocation);
    }
    
    std::pair<VkBuffer, VmaAllocation> Device::AllocateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_flags, VmaAllocationCreateFlags allocation_flags) {
        return AllocateBuffer(size, usage, {}, memory_flags, allocation_flags);
    }
    
    void Device::FreeBuffer(VkBuffer buffer, VmaAllocation allocation) {
        vmaDestroyBuffer(m_allocator, buffer, allocation);
    }
    
    std::pair<VkImage, VmaAllocation> Device::AllocateImage(VkFormat format, VkExtent2D extent, VkImageUsageFlags usage, std::vector<QueueType> shared_queues, uint32_t mipmap_levels, VkMemoryPropertyFlags memory_flags, VmaAllocationCreateFlags allocation_flags) {        
        VkImageCreateInfo image_create_info {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .extent = { .width = extent.width, .height = extent.height, .depth = 1 },
            .mipLevels = mipmap_levels,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
        };
    
        _SetSharingMode(image_create_info, shared_queues, this);
    
        VmaAllocationCreateInfo allocation_create_info {
            .usage = VMA_MEMORY_USAGE_AUTO,
            .requiredFlags = memory_flags,
            .flags = allocation_flags,
        };
    
        VkImage image;
        VmaAllocation allocation;
        VK_CHECK(vmaCreateImage(m_allocator, &image_create_info, &allocation_create_info, &image, &allocation, nullptr));
    
        return std::make_pair(image, allocation);
    }
    
    std::pair<VkImage, VmaAllocation> Device::AllocateImage(VkFormat format, VkExtent2D extent, VkImageUsageFlags usage, uint32_t mipmap_levels, VkMemoryPropertyFlags memory_flags, VmaAllocationCreateFlags allocation_flags) {
        return AllocateImage(format, extent, usage, {}, mipmap_levels, memory_flags, allocation_flags);
    }
    
    void Device::FreeImage(VkImage image, VmaAllocation allocation) {
        vmaDestroyImage(m_allocator, image, allocation);
    }
    
    VkImageView Device::CreateImageView(VkImage image, VkFormat format, VkImageSubresourceRange range, VkImageViewType image_view_type) const {
        VkImageViewCreateInfo color_attachment_view = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .format = format,
            .components = {
                VK_COMPONENT_SWIZZLE_R,
                VK_COMPONENT_SWIZZLE_G,
                VK_COMPONENT_SWIZZLE_B,
                VK_COMPONENT_SWIZZLE_A
            },
            .subresourceRange = range,
            .viewType = image_view_type,
            .image = image,
        };
    
        VkImageView image_view;
        VK_CHECK(vkCreateImageView(m_device, &color_attachment_view, nullptr, &image_view));
        return image_view;
    }
    
    void Device::DestroyImageView(VkImageView image_view) const {
        vkDestroyImageView(m_device, image_view, nullptr);
    }

    VmaAllocationInfo Device::GetAllocationInfo(VmaAllocation allocation) {
        VmaAllocationInfo alloc_info_out;
        vmaGetAllocationInfo(m_allocator, allocation, &alloc_info_out);
        return alloc_info_out;
    }
    
    VkCommandPool Device::CreateCommandPool(QueueType queue) {
        VkCommandPoolCreateInfo command_pool_create_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = GetQueueIndex(queue),
        };
    
        VkCommandPool command_pool;
        VK_CHECK(vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &command_pool));
        return command_pool;
    }
    
    void Device::DestroyCommandPool(VkCommandPool command_pool) {
        vkDestroyCommandPool(m_device, command_pool, nullptr);
    }
    
    VkCommandBuffer Device::AllocateCommandBuffer(VkCommandPool command_pool) {
       return AllocateCommandBuffers(command_pool, 1)[0];
    }
    
    std::vector<VkCommandBuffer> Device::AllocateCommandBuffers(VkCommandPool command_pool, uint32_t count) {
        VkCommandBufferAllocateInfo command_buffer_allocate_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = command_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = count,
        };
    
        std::vector<VkCommandBuffer> command_buffers(count);
        VK_CHECK(vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, command_buffers.data()))
        return command_buffers;
    }
    
    VkPipelineLayout Device::CreatePipelineLayout(const std::vector<VkDescriptorSetLayout> &descriptor_set_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges) {
        VkPipelineLayout pipeline_layout;
    
        VkPipelineLayoutCreateInfo pipeline_layout_create_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size()),
            .pSetLayouts = descriptor_set_layouts.data(),
            .pushConstantRangeCount = static_cast<uint32_t>(push_constant_ranges.size()),
            .pPushConstantRanges = push_constant_ranges.data(),
        };
    
        VK_CHECK(vkCreatePipelineLayout(m_device, &pipeline_layout_create_info, nullptr, &pipeline_layout));
    
        return pipeline_layout;
    }
    
    void Device::DestroyPipelineLayout(VkPipelineLayout pipeline_layout) {
        vkDestroyPipelineLayout(m_device, pipeline_layout, nullptr);
    }
    
    VkShaderModule Device::CreateShaderModule(const std::string &shader_path) {
        
        std::ifstream file(shader_path, std::ios::ate | std::ios::binary); 
        if (!file.is_open()) {
            throw std::runtime_error("Could not find shader at path: " + shader_path);
        }
    
    
        size_t file_size = static_cast<size_t>(file.tellg());
        if (file_size % 4 != 0) {
            throw std::runtime_error("Shader file not byte-aligned. Are you sure this is a SPIR-V file?");
        }
        std::vector<char> source_buffer(file_size);
    
        file.seekg(0);
        file.read(source_buffer.data(), file_size);
        file.close();
    
        VkShaderModuleCreateInfo shader_module_create_info {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .codeSize = source_buffer.size(),
            .pCode    = reinterpret_cast<const uint32_t *>(source_buffer.data()),
        };
    
        VkShaderModule shader_module;
    
        VK_CHECK(vkCreateShaderModule(m_device, &shader_module_create_info, nullptr, &shader_module));
    
        return shader_module;
    }
    
    void Device::DestroyShaderModule(VkShaderModule shader_module) {
        vkDestroyShaderModule(m_device, shader_module, nullptr);
    }
    
    void Device::DestroyPipeline(VkPipeline pipeline) {
        vkDestroyPipeline(m_device, pipeline, nullptr);
    }

}
