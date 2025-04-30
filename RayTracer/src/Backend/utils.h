#pragma once

#include <volk.h>
#include <string>
#include <vector>

namespace VulkanBackend {
    
    /**
     * @brief Commands to transition an image from its current layout to a new one.
     * 
     * This method should only be called whenever the command buffer is actively recording for the
     * current frame since it works as a convenience method to use `vkCmdPipelineBarrier` for its
     * use in transitioning image layouts.
     */
    void TransitionImageLayout(
        VkCommandBuffer command_buffer, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout,
        VkPipelineStageFlags2 src_stage, VkPipelineStageFlags2 dst_stage,
        VkAccessFlags2 src_access = VK_ACCESS_2_MEMORY_WRITE_BIT, VkAccessFlags2 dst_access = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT
    );
    
    /**
     * @brief Copies an image to another image.
     * 
     * @param filter Defines how to handle differing resolutions.
     * 
     * Currently only supports colored images (i.e. not depths or otherwise). It is best to use a transfer command buffer to handle this
     * operation. We assume that:
     *  - command_buffer is a valid buffer that has been allocated
     *  - src_image has layout VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
     *  - dst_image has layout VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
     *  - src_image and dst_image have compatible formats for copying
     *  - If different resolution, src_image and dst_image must be compatible with filter
     */
    void BlitImageToImage(VkCommandBuffer command_buffer, VkImage src_image, VkImage dst_image, VkExtent2D src_size, VkExtent2D dst_size, VkFilter filter = VK_FILTER_LINEAR);
    
    void CopyImageToImage(VkCommandBuffer command_buffer, VkImage src_image, VkImage dst_image, VkExtent2D size);
    
    void SubmitQueue(
        VkQueue queue,
        VkCommandBuffer command_buffer,
        std::vector<VkSemaphore> wait_semaphores,
        std::vector<VkSemaphore> signal_semaphores,
        VkFence signal_fence,
        VkPipelineStageFlags2 wait_stage,
        VkPipelineStageFlags2 signal_stage
    );
    
    void SubmitQueue(
        VkQueue queue,
        const std::vector<VkCommandBuffer> &command_buffers,
        const std::vector<VkSemaphore> &wait_semaphores,
        const std::vector<VkSemaphore> &signal_semaphores,
        VkFence signal_fence,
        VkPipelineStageFlags2 wait_stage,
        VkPipelineStageFlags2 signal_stage
    );

}
