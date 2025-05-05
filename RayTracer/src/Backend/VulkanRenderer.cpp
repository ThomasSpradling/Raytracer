#include "VulkanRenderer.h"
#include "Backend/debug.h"
#include "Backend/utils.h"
#include "Utils/Profiler.h"
#include <iostream>

namespace VulkanBackend {

    VulkanRenderer::VulkanRenderer(Platform::Window &window)
        : m_window(window)
    {
        std::cout << "Setting up Vulkan renderer." << std::endl;

        VK_CHECK(volkInitialize());
        InitVulkan();
        PrepareFrameData();
    }

    VulkanRenderer::~VulkanRenderer() {
        std::cout << "Destroying up Vulkan renderer." << std::endl;

        vkDeviceWaitIdle(m_context.device->GetDevice());

        for (int i = 0; i < MAX_CONCURRENT_FRAMES; ++i) {
            m_context.device->DestroySemaphore(m_frame_data[i].transfer_complete_semaphore);
            m_context.device->DestroySemaphore(m_frame_data[i].swapchain_acquire_semaphore);
            m_context.device->DestroyFence(m_frame_data[i].render_fence);
            m_context.device->DestroyCommandPool(m_frame_data[i].graphics_command_pool);
            m_context.device->DestroyCommandPool(m_frame_data[i].transfer_command_pool);

            m_context.device->FreeBuffer(m_frame_data[i].staging_buffer, m_frame_data[i].staging_allocation);
        }

        m_context.swapchain.reset();
        m_context.instance->DestroyPresentSurface(m_context.surface);
        m_context.device.reset();
        m_context.instance.reset();
    }
    
    void VulkanRenderer::Present(const std::vector<uint8_t> &data, uint32_t width, uint32_t height) {
        PROFILE_FUNCTION_AUTO();
        // Set up imgui for this frame
        
        const PerFrameData &current_frame = m_frame_data[m_current_frame_index];
        
        m_context.device->WaitForFence(current_frame.render_fence);
        m_context.device->ResetFence(current_frame.render_fence);
    
        uint32_t swapchain_image_index = m_context.swapchain->AcquireNextImageIndex(current_frame.swapchain_acquire_semaphore);

        VkImage swapchain_image = (*m_context.swapchain)[swapchain_image_index];

        // Uploading `data` to GPU //

        std::memcpy(current_frame.staging_map, data.data(), data.size());

        // Recording Command Buffers //

        VkCommandBufferBeginInfo command_buffer_begin_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr,
        };
        

        VK_CHECK(vkBeginCommandBuffer(current_frame.transfer_command_buffer, &command_buffer_begin_info));
            // Record commands to transfer data into swapchain image
            TransitionImageLayout(
                current_frame.transfer_command_buffer,
                swapchain_image,
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT);

            VkBufferImageCopy copy_region {
                .bufferOffset = 0,
                .bufferRowLength = 0,
                .bufferImageHeight = 0,
                .imageSubresource = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
                .imageOffset = { 0, 0, 0 },
                .imageExtent = { width, height, 1 },
            };
            vkCmdCopyBufferToImage(
                current_frame.transfer_command_buffer,
                current_frame.staging_buffer,
                swapchain_image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &copy_region
            );

            TransitionImageLayout(
                current_frame.transfer_command_buffer,
                swapchain_image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT
            );
        VK_CHECK(vkEndCommandBuffer(current_frame.transfer_command_buffer));

        // VK_CHECK(vkBeginCommandBuffer(current_frame.graphics_command_buffer, &command_buffer_begin_info));
        //     // Record commands to draw ImGui components
        // VK_CHECK(vkEndCommandBuffer(current_frame.graphics_command_buffer));
        
        SubmitQueue(
            m_context.transfer_queue,
            current_frame.transfer_command_buffer,
            { current_frame.swapchain_acquire_semaphore },     // <-- wait
            { current_frame.transfer_complete_semaphore },    // <-- signal
            current_frame.render_fence,
            VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT
        );

        m_context.swapchain->Present(
            swapchain_image_index,
            { current_frame.transfer_complete_semaphore }    // <-- wait
        );

        m_current_frame_index = (m_current_frame_index + 1) % MAX_CONCURRENT_FRAMES;
    }

    void VulkanRenderer::InitVulkan() {
        // TODO: Set this somewhere else
        const std::string application_name = "Example Application";
        const bool enable_validation_layers = true;

        #ifdef __APPLE__
            const VulkanInstance::APIVersion version = VulkanInstance::APIVersion::version_molten_vk;
        #else
            const VulkanInstance::APIVersion version = VulkanInstance::APIVersion::version_1_3;
        #endif

        VkExtent2D requested_swapchain_extent = m_window.GetWindowExtent();

        m_context.instance = std::make_unique<VulkanInstance>(application_name, version, enable_validation_layers);
        
        m_context.surface = m_context.instance->CreatePresentSurface(m_window);
        m_context.device = std::make_unique<Device>(*m_context.instance, m_context.surface);

        m_context.swapchain = std::make_unique<SwapChain>(*m_context.device, m_context.surface, requested_swapchain_extent);

        m_context.graphics_queue = m_context.device->GetQueue(Device::QueueType::graphics);
        m_context.present_queue = m_context.device->GetQueue(Device::QueueType::present);
        m_context.compute_queue = m_context.device->GetQueue(Device::QueueType::compute);
        m_context.transfer_queue = m_context.device->GetQueue(Device::QueueType::transfer);
    }

    void VulkanRenderer::PrepareFrameData() {
        for (int i = 0; i < MAX_CONCURRENT_FRAMES; ++i) {
            m_frame_data[i].render_fence = m_context.device->CreateFence(true);
            m_frame_data[i].swapchain_acquire_semaphore = m_context.device->CreateSemaphore();
            m_frame_data[i].transfer_complete_semaphore = m_context.device->CreateSemaphore();
            // m_frame_data[i].draw_complete_semaphore = m_context.device->CreateSemaphore();

            m_frame_data[i].graphics_command_pool = m_context.device->CreateCommandPool(Device::QueueType::graphics);
            m_frame_data[i].graphics_command_buffer = m_context.device->AllocateCommandBuffer(m_frame_data[i].graphics_command_pool);

            m_frame_data[i].transfer_command_pool = m_context.device->CreateCommandPool(Device::QueueType::transfer);
            m_frame_data[i].transfer_command_buffer = m_context.device->AllocateCommandBuffer(m_frame_data[i].transfer_command_pool);

            // m_frame_data[i].compute_command_pool = m_context.device->CreateCommandPool(Device::QueueType::compute);
            // m_frame_data[i].compute_command_buffer = m_context.device->AllocateCommandBuffer(m_frame_data[i].compute_command_pool);
        
            // [m_frame_data[i].staging_buffer, m_frame_data[i].staging_allocation] = m_d

            VkDeviceSize staging_size =
                m_context.swapchain->GetExtent().width *
                m_context.swapchain->GetExtent().height *
                4;
            
            auto [staging_buffer, staging_allocation] = m_context.device->AllocateBuffer(
                staging_size,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                    | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                VMA_ALLOCATION_CREATE_MAPPED_BIT
                    | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
            );
            
            m_frame_data[i].staging_buffer = staging_buffer;
            m_frame_data[i].staging_allocation = staging_allocation;

            VmaAllocationInfo allocation_info = m_context.device->GetAllocationInfo(staging_allocation);
            m_frame_data[i].staging_map = allocation_info.pMappedData;
        }
    }

}

