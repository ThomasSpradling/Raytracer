#pragma once

#include "Backend/Device.h"
#include "Backend/SwapChain.h"
#include "Backend/VulkanInstance.h"
#include "Common/Color.h"
#include <memory>
#include <vector>

namespace VulkanBackend {

    /**
     * @brief Handles the direct low-level rendering of an image to
     * the Vulkan swap chain. It will initialize the Vulkan instance,
     * device, queues, and swap chain.
     */
    class VulkanRenderer {
    public:
        VulkanRenderer(Platform::Window &window);
        ~VulkanRenderer();

        void Present(const std::vector<uint8_t> &data, uint32_t width, uint32_t height);
        VkFormat SwapChainFormat() { return m_context.swapchain->GetFormat(); }
    private:
        struct Context {
            std::unique_ptr<VulkanInstance> instance;
            std::unique_ptr<Device> device;

            VkSurfaceKHR surface = VK_NULL_HANDLE;
            std::unique_ptr<SwapChain> swapchain;

            VkQueue graphics_queue;
            VkQueue present_queue;
            VkQueue compute_queue;
            VkQueue transfer_queue;
        };

        struct PerFrameData {
            VkFence render_fence = VK_NULL_HANDLE;
            VkSemaphore swapchain_acquire_semaphore = VK_NULL_HANDLE;
            VkSemaphore transfer_complete_semaphore = VK_NULL_HANDLE;

            VkCommandPool graphics_command_pool;
            VkCommandBuffer graphics_command_buffer;

            VkCommandPool transfer_command_pool;
            VkCommandBuffer transfer_command_buffer;

            VkBuffer staging_buffer;
            VmaAllocation staging_allocation;
            void *staging_map;
        };

        static constexpr uint32_t MAX_CONCURRENT_FRAMES = 2;
    private:
        Context m_context;
        uint32_t m_current_frame_index = 0;

        Platform::Window &m_window;

        std::array<PerFrameData, MAX_CONCURRENT_FRAMES> m_frame_data;
    private:
        void InitVulkan();
        void PrepareFrameData();
    };

}
