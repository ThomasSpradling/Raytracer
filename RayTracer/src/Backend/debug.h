#pragma once

#include <volk.h>
#include <vulkan/vk_enum_string_helper.h>
#include <stdexcept>
#include <format>

#define VK_CHECK(expr) \
    if (VkResult result = expr; result != VK_SUCCESS) { \
        throw std::runtime_error(std::format("Call '{}' returned {}.", #expr, string_VkResult(result))); \
    }
