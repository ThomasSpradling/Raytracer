#pragma once

#include "Common/NonCopyable.h"
#include "Common/NonMovable.h"
#include <optional>
#include "VulkanInstance.h"
#include "vk_mem_alloc.h"

namespace VulkanBackend {
    
    class Device : private NonCopyable, private NonMovable {
    public:
        enum class QueueType {
            graphics,
            present,
            transfer,
            compute,
        };
    
        struct Queue {
            VkQueue queue = VK_NULL_HANDLE;
            uint32_t family_index = -1;
        };
    
        static constexpr VkImageSubresourceRange DEFAULT_COLOR_IMAGE_SUBRESOURCE = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };
    
        static constexpr VkImageSubresourceRange DEFAULT_DEPTH_IMAGE_SUBRESOURCE = {
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };
        
        static constexpr VkImageSubresourceRange FULL_TEXTURE_IMAGE_SUBRESOURCE = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = VK_REMAINING_MIP_LEVELS,
            .baseArrayLayer = 0,
            .layerCount = VK_REMAINING_ARRAY_LAYERS,
        };
    public:
        Device(const VulkanInstance &instance, VkSurfaceKHR surface);
        ~Device();
    
        VkQueue GetQueue(QueueType queue) const;
        uint32_t GetQueueIndex(QueueType queue) const;
    
        VkSemaphore CreateSemaphore();
        void DestroySemaphore(VkSemaphore semaphore);
    
        VkFence CreateFence(bool signalled = true);
        void DestroyFence(VkFence fence);
        void WaitForFence(VkFence fences, uint64_t timeout = DEFAULT_FENCE_TIMEOUT_TIME);
        void WaitForAllFences(const std::vector<VkFence> &fences, uint64_t timeout = DEFAULT_FENCE_TIMEOUT_TIME);
        void WaitForOneFence(const std::vector<VkFence> &fences, uint64_t timeout = DEFAULT_FENCE_TIMEOUT_TIME);
        void ResetFence(VkFence fence);
        void ResetFences(const std::vector<VkFence> &fences);
    
        std::pair<VkBuffer, VmaAllocation> AllocateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, std::vector<QueueType> shared_queues, VkMemoryPropertyFlags memory_flags, VmaAllocationCreateFlags allocation_flags = 0);   
        std::pair<VkBuffer, VmaAllocation> AllocateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_flags, VmaAllocationCreateFlags allocation_flags = 0);
        void FreeBuffer(VkBuffer buffer, VmaAllocation allocation);
        
        std::pair<VkImage, VmaAllocation> AllocateImage(VkFormat format, VkExtent2D extent, VkImageUsageFlags usage, std::vector<QueueType> shared_queues, uint32_t mipmap_levels = 1, VkMemoryPropertyFlags memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VmaAllocationCreateFlags allocation_flags = 0);
        std::pair<VkImage, VmaAllocation> AllocateImage(VkFormat format, VkExtent2D extent, VkImageUsageFlags usage, uint32_t mipmap_levels = 1, VkMemoryPropertyFlags memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VmaAllocationCreateFlags allocation_flags = 0);
        void FreeImage(VkImage image, VmaAllocation allocation);
                
        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageSubresourceRange range = DEFAULT_COLOR_IMAGE_SUBRESOURCE, VkImageViewType image_view_type = VK_IMAGE_VIEW_TYPE_2D) const;
        void DestroyImageView(VkImageView image_view) const;

        VmaAllocationInfo GetAllocationInfo(VmaAllocation allocation);
    
        VkCommandPool CreateCommandPool(QueueType queue);
        void DestroyCommandPool(VkCommandPool command_pool);
    
        VkCommandBuffer AllocateCommandBuffer(VkCommandPool command_pool);
        std::vector<VkCommandBuffer> AllocateCommandBuffers(VkCommandPool command_pool, uint32_t count);
    
        VkPipelineLayout CreatePipelineLayout(const std::vector<VkDescriptorSetLayout> &descriptor_set_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges);
        void DestroyPipelineLayout(VkPipelineLayout pipeline_layout);
    
        VkShaderModule CreateShaderModule(const std::string &shader_path);
        void DestroyShaderModule(VkShaderModule shader_module);
    
        void DestroyPipeline(VkPipeline pipeline);
        
        inline VkPhysicalDevice GetPhysicalDevice() const { return m_physical_device; }
        inline VkDevice GetDevice() const { return m_device; }
        
    private:
        static constexpr uint64_t DEFAULT_FENCE_TIMEOUT_TIME = 1'000'000'000; // 1 second
    private:
        std::optional<Queue> m_graphics_queue;    
        std::optional<Queue> m_present_queue;    
        std::optional<Queue> m_transfer_queue;    
        std::optional<Queue> m_compute_queue;
    
        const VulkanInstance &m_instance;
        VkPhysicalDevice m_physical_device;
        VkDevice m_device;
    
        VmaAllocator m_allocator;
    private:
        Device::Queue _GetQueue(QueueType queue) const;
    };

}
