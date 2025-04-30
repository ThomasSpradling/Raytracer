#include "SwapChain.h"
#include "debug.h"
#include <format>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace VulkanBackend {
    
    SwapChain::SwapChain(const Device &device, VkSurfaceKHR surface, VkExtent2D extent, bool vsync)
        : m_device(device)
        , m_surface(surface)
        , m_vsync_enabled(vsync)
    {
        std::cout << "  Creating swap chain" << std::endl;
    
        m_swapchain_format = ChooseSurfaceFormat();
        m_swapchain_present_mode = ChoosePresentMode(m_vsync_enabled);
    
        Create(extent);
    }
    
    SwapChain::~SwapChain() {
        std::cout << "  Destroying swap chain" << std::endl;
    
        Destroy();
    }
    
    void SwapChain::Create(VkExtent2D extent) {
        VkSwapchainKHR old_swapchain = m_swapchain;
    
        VkPhysicalDevice physical_device = m_device.GetPhysicalDevice();
        VkSurfaceCapabilitiesKHR surface_capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, m_surface, &surface_capabilities);
    
        m_swapchain_extent = ChooseSwapChainExtent(surface_capabilities, extent);
    
        uint32_t min_image_count = ChooseImageCount(surface_capabilities);
        VkSurfaceTransformFlagsKHR pre_transform = ChoosePreTransform(surface_capabilities);
        VkCompositeAlphaFlagBitsKHR composite_alpha = ChooseCompositeAlpha(surface_capabilities);
    
        VkSwapchainCreateInfoKHR swapchain_create_info {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .surface = m_surface,
            .minImageCount = min_image_count,
            .imageFormat = m_swapchain_format.format,
            .imageColorSpace = m_swapchain_format.colorSpace,
            .imageExtent = m_swapchain_extent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(pre_transform),
            .compositeAlpha = composite_alpha,
            .presentMode = m_swapchain_present_mode,
            .clipped = VK_TRUE,
            .oldSwapchain = old_swapchain,
        };
    
        std::set<uint32_t> unique_queue_indices = {
            m_device.GetQueueIndex(Device::QueueType::graphics),
            m_device.GetQueueIndex(Device::QueueType::present),
        };
        std::vector<uint32_t> queue_family_indices(unique_queue_indices.begin(), unique_queue_indices.end());
    
        if (unique_queue_indices.size() > 1) {
            swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchain_create_info.queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size());
            swapchain_create_info.pQueueFamilyIndices = queue_family_indices.data();
        }
    
        assert(surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
        swapchain_create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        
        assert(surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT);
        swapchain_create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    
        VK_CHECK(vkCreateSwapchainKHR(m_device.GetDevice(), &swapchain_create_info, nullptr, &m_swapchain));
    
        if (old_swapchain != VK_NULL_HANDLE) {
            for (int i = 0; i < m_images.size(); ++i) {
                m_device.DestroyImageView(m_image_views[i]);
            }
            vkDestroySwapchainKHR(m_device.GetDevice(), old_swapchain, nullptr);
        }
    
        uint32_t image_count = 0;
        VK_CHECK(vkGetSwapchainImagesKHR(m_device.GetDevice(), m_swapchain, &image_count, nullptr));
    
        m_images.resize(image_count);
        VK_CHECK(vkGetSwapchainImagesKHR(m_device.GetDevice(), m_swapchain, &image_count, m_images.data()));
    
        m_image_views.resize(image_count);
        for (int i = 0; i < m_images.size(); ++i) {
            m_image_views[i] = m_device.CreateImageView(m_images[i], m_swapchain_format.format);
        }
    }
    
    void SwapChain::Destroy() {
        for (int i = 0; i < m_images.size(); ++i) {
            vkDestroyImageView(m_device.GetDevice(), m_image_views[i], nullptr);
        }
        vkDestroySwapchainKHR(m_device.GetDevice(), m_swapchain, nullptr);
    }
    
    void SwapChain::Resize(VkExtent2D extent) {
        m_resize_requested = false;
    
        Create(extent);
    }
    
    VkImage SwapChain::operator[](uint32_t image_index) noexcept {
        return m_images[image_index];
    }
    
    uint32_t SwapChain::AcquireNextImageIndex(VkSemaphore signal_semaphore, VkFence signal_fence) {
        uint32_t image_index;
        VkResult res = vkAcquireNextImageKHR(
            m_device.GetDevice(),
            m_swapchain,
            DEFAULT_SWAPCHAIN_ACQUIRE_TIMEOUT_TIME,
            signal_semaphore,
            signal_fence,
            &image_index
        );
    
        if (res == VK_ERROR_OUT_OF_DATE_KHR) {
            std::cout << "ACQUIRE NEXT IMAGE RESIZE" << std::endl;
            m_resize_requested = true;
            return UINT32_MAX;
        }
    
        if (res == VK_SUBOPTIMAL_KHR) {
            return image_index;
        }
    
        VK_CHECK(res);
        return image_index;
    }
    
    void SwapChain::Present(uint32_t swapchain_image_index, const std::vector<VkSemaphore> &wait_semaphores) {
        VkPresentInfoKHR present_info = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = static_cast<uint32_t>(wait_semaphores.size()),
            .pWaitSemaphores = wait_semaphores.data(),
            .swapchainCount = 1,
            .pSwapchains = &m_swapchain,
            .pImageIndices = &swapchain_image_index,
            .pResults = nullptr,
        };
    
        VkResult res = vkQueuePresentKHR(m_device.GetQueue(Device::QueueType::present), &present_info);
        if (res == VK_ERROR_OUT_OF_DATE_KHR  || res == VK_SUBOPTIMAL_KHR) {
            m_resize_requested = true;
        } else {
            VK_CHECK(res);
        }
    }
    
    VkExtent2D SwapChain::ChooseSwapChainExtent(const VkSurfaceCapabilitiesKHR &surface_capabilities, VkExtent2D desired_extent) {    
        if (surface_capabilities.currentExtent.width == UINT32_MAX) {
            VkExtent2D actual_extent = desired_extent;
            actual_extent.width = std::clamp(actual_extent.width, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width);
            actual_extent.height = std::clamp(actual_extent.height, surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height);
            return actual_extent;
        }
    
        return surface_capabilities.currentExtent;
    }
    
    VkSurfaceFormatKHR SwapChain::ChooseSurfaceFormat() {
        VkPhysicalDevice physical_device = m_device.GetPhysicalDevice();
    
        uint32_t surface_formats_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, m_surface, &surface_formats_count, nullptr);
        assert(surface_formats_count > 0);
        std::vector<VkSurfaceFormatKHR> surface_formats(surface_formats_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, m_surface, &surface_formats_count, surface_formats.data());
    
        for (const auto &format : surface_formats) {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return format;
            }
        }
    
        return surface_formats[0];
    }
    
    VkPresentModeKHR SwapChain::ChoosePresentMode(bool vsync) {
        VkPhysicalDevice physical_device = m_device.GetPhysicalDevice();
        
        uint32_t present_modes_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, m_surface, &present_modes_count, nullptr);
        assert(present_modes_count > 0);
        std::vector<VkPresentModeKHR> present_modes(present_modes_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, m_surface, &present_modes_count, present_modes.data());
    
        if (vsync)
            return VK_PRESENT_MODE_FIFO_KHR;
    
        for (const auto &mode : present_modes) {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return mode;
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    
    uint32_t SwapChain::ChooseImageCount(const VkSurfaceCapabilitiesKHR &surface_capabilities) {
        // Choose image count. If max image count is zero, then we support any number of images
        uint32_t image_count = surface_capabilities.minImageCount + 1;
        if (surface_capabilities.maxImageCount > 0 && image_count > surface_capabilities.maxImageCount) {
            image_count = surface_capabilities.maxImageCount;
        }
    
        return image_count;
    }
    
    VkSurfaceTransformFlagsKHR SwapChain::ChoosePreTransform(const VkSurfaceCapabilitiesKHR &surface_capabilities) {
        VkSurfaceTransformFlagsKHR pre_transform;
        if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
            return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        }
        return surface_capabilities.currentTransform;
    }
    
    VkCompositeAlphaFlagBitsKHR SwapChain::ChooseCompositeAlpha(const VkSurfaceCapabilitiesKHR &surface_capabilities) {
        // We bias opaque composite alpha format, but we choose whichever is supported
        // first in this list:
        std::vector<VkCompositeAlphaFlagBitsKHR> composite_alpha_flags {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        };
    
        for (auto flag : composite_alpha_flags) {
            if (surface_capabilities.supportedCompositeAlpha & flag) {
                return flag;
            }
        }
    
        throw std::runtime_error("Invalid composite alpha!");
    }

}
