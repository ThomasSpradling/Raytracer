#include "utils.h"
#include "debug.h"
#include "Device.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace VulkanBackend {
    
    void TransitionImageLayout(
        VkCommandBuffer command_buffer, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout,
        VkPipelineStageFlags2 src_stage, VkPipelineStageFlags2 dst_stage,
        VkAccessFlags2 src_access, VkAccessFlags2 dst_access) {
        VkImageMemoryBarrier2 image_barrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext = nullptr,
            .srcStageMask = src_stage,
            .srcAccessMask = src_access,
            .dstStageMask = dst_stage,
            .dstAccessMask = dst_access,
            
            .oldLayout = old_layout,
            .newLayout = new_layout,
    
            .subresourceRange = (new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
                ? Device::DEFAULT_DEPTH_IMAGE_SUBRESOURCE : Device::DEFAULT_COLOR_IMAGE_SUBRESOURCE,
            .image = image,
        };
    
        VkDependencyInfo dependency_info {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .pNext = nullptr,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &image_barrier,
        };
        vkCmdPipelineBarrier2(command_buffer, &dependency_info);
    }
    
    void BlitImageToImage(VkCommandBuffer command_buffer, VkImage src_image, VkImage dst_image, VkExtent2D src_size, VkExtent2D dst_size, VkFilter filter) {
        VkImageSubresourceLayers subresource_layers = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseArrayLayer = 0,
            .layerCount = 1,
            .mipLevel = 0,
        };
    
        VkImageBlit2 region {
            .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
            .pNext = nullptr,
            .srcOffsets = {
                { 0, 0, 0 },
                { static_cast<int32_t>(src_size.width), static_cast<int32_t>(src_size.height), 1 }
            },
            .dstOffsets = {
                { 0, static_cast<int32_t>(dst_size.height), 0 },
                { static_cast<int32_t>(dst_size.width), 0, 1 }
            },
            .srcSubresource = subresource_layers,
            .dstSubresource = subresource_layers,
        };
    
        VkBlitImageInfo2 blit_image_info {
            .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
            .pNext = nullptr,
            .srcImage = src_image,
            .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .dstImage = dst_image,
            .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .regionCount = 1,
            .pRegions = &region,
            .filter = filter,
        };
    
        vkCmdBlitImage2KHR(command_buffer, &blit_image_info);
    }
    
    void CopyImageToImage(VkCommandBuffer command_buffer, VkImage src_image, VkImage dst_image, VkExtent2D size) {
        VkImageSubresourceLayers subresource_layers = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseArrayLayer = 0,
            .layerCount = 1,
            .mipLevel = 0,
        };
        
        VkImageCopy2 region {
            .sType = VK_STRUCTURE_TYPE_IMAGE_COPY_2,
            .pNext = nullptr,
            .srcSubresource = subresource_layers,
            .dstSubresource = subresource_layers,
            .extent = { size.width, size.height, 1 },
        };
    
        VkCopyImageInfo2 copy_image_info {
            .sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2,
            .pNext = nullptr,
            .srcImage = src_image,
            .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .dstImage = dst_image,
            .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .regionCount = 1,
            .pRegions = &region,
        };
        vkCmdCopyImage2KHR(command_buffer, &copy_image_info);    
    }
    
    void SubmitQueue(
        VkQueue queue,
        const std::vector<VkCommandBuffer> &command_buffers,
        const std::vector<VkSemaphore> &wait_semaphores,
        const std::vector<VkSemaphore> &signal_semaphores,
        VkFence signal_fence,
        VkPipelineStageFlags2 wait_stage,
        VkPipelineStageFlags2 signal_stage
    ) {
        std::vector<VkCommandBufferSubmitInfo> command_buffer_submit_infos;
        command_buffer_submit_infos.reserve(command_buffers.size());
        
        for (auto command_buffer : command_buffers) {
            command_buffer_submit_infos.push_back({
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                .pNext = nullptr,
                .commandBuffer = command_buffer,
                .deviceMask = 1,
            });
        }
    
        std::vector<VkSemaphoreSubmitInfo> wait_semaphore_submit_infos;
        wait_semaphore_submit_infos.reserve(wait_semaphores.size());
        
        for (auto semaphore : wait_semaphores) {
            wait_semaphore_submit_infos.push_back({
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .pNext = nullptr,
                .semaphore = semaphore,
                .stageMask = wait_stage,
                .deviceIndex = 0,
                .value = 1,
            });
        }
    
        std::vector<VkSemaphoreSubmitInfo> signal_semaphore_submit_infos;
        signal_semaphore_submit_infos.reserve(signal_semaphores.size());
    
        for (auto semaphore : signal_semaphores) {
            signal_semaphore_submit_infos.push_back({
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .pNext = nullptr,
                .semaphore = semaphore,
                .stageMask = signal_stage,
                .deviceIndex = 0,
                .value = 1,
            });
        }
    
        VkSubmitInfo2KHR submit_info {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .pNext = nullptr,
            .waitSemaphoreInfoCount = static_cast<uint32_t>(wait_semaphore_submit_infos.size()),
            .pWaitSemaphoreInfos = wait_semaphore_submit_infos.data(),
            .commandBufferInfoCount = static_cast<uint32_t>(command_buffer_submit_infos.size()),
            .pCommandBufferInfos = command_buffer_submit_infos.data(),
            .signalSemaphoreInfoCount = static_cast<uint32_t>(signal_semaphore_submit_infos.size()),
            .pSignalSemaphoreInfos = signal_semaphore_submit_infos.data(),
        };
    
        VK_CHECK(vkQueueSubmit2KHR(queue, 1, &submit_info, signal_fence));
    }
    
    void SubmitQueue(
        VkQueue queue,
        VkCommandBuffer command_buffer,
        std::vector<VkSemaphore> wait_semaphores,
        std::vector<VkSemaphore> signal_semaphores,
        VkFence signal_fence,
        VkPipelineStageFlags2 wait_stage,
        VkPipelineStageFlags2 signal_stage
    ) {
        std::vector<VkCommandBuffer> buffers = { command_buffer };
        SubmitQueue(queue, buffers, wait_semaphores, signal_semaphores, signal_fence, wait_stage, signal_stage);
    }

}