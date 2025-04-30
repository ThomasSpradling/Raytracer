#pragma once

#include "Device.h"
#include "Common/NonCopyable.h"
#include "Common/NonMovable.h"
#include <vector>

namespace VulkanBackend {
    
    class SwapChain : private NonCopyable, private NonMovable {
    public:
        SwapChain(const Device &device, VkSurfaceKHR surface, VkExtent2D extent, bool vsync = false);
        ~SwapChain();
    
        void Create(VkExtent2D extent);
        void Destroy();
    
        void Resize(VkExtent2D extent);
    
        inline const VkSwapchainKHR &GetSwapChain() { return m_swapchain; }
    
        inline VkExtent2D GetExtent() const { return m_swapchain_extent; }
        inline VkFormat GetFormat() const { return m_swapchain_format.format; }
        
        VkImage operator[](uint32_t image_index) noexcept;
    
        inline bool ResizeRequested() { return m_resize_requested; }
    
        uint32_t AcquireNextImageIndex(VkSemaphore signal_semaphore = VK_NULL_HANDLE, VkFence signal_fence = VK_NULL_HANDLE);
        void Present(uint32_t swapchain_image_index, const std::vector<VkSemaphore> &wait_semaphores);
    private:
        static constexpr uint64_t DEFAULT_SWAPCHAIN_ACQUIRE_TIMEOUT_TIME = 1'000'000'000; // 1 second
    
        const Device &m_device;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;

        VkFormat m_format;
        
        std::vector<VkImage> m_images;
        std::vector<VkImageView> m_image_views;
    
        VkSurfaceFormatKHR m_swapchain_format;
        VkPresentModeKHR m_swapchain_present_mode;
        VkExtent2D m_swapchain_extent;
    
        bool m_resize_requested = false;
        bool m_vsync_enabled = false;
    private:
        VkExtent2D ChooseSwapChainExtent(const VkSurfaceCapabilitiesKHR &surface_capabilities, VkExtent2D desired_extent);
        VkSurfaceFormatKHR ChooseSurfaceFormat();
        VkPresentModeKHR ChoosePresentMode(bool vsync);
        uint32_t ChooseImageCount(const VkSurfaceCapabilitiesKHR &surface_capabilities);
        VkSurfaceTransformFlagsKHR ChoosePreTransform(const VkSurfaceCapabilitiesKHR &surface_capabilities);
        VkCompositeAlphaFlagBitsKHR ChooseCompositeAlpha(const VkSurfaceCapabilitiesKHR &surface_capabilities);
    };

}
