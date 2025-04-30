#pragma once

#include "Backend/VulkanRenderer.h"
#include "Platform/Window.h"

namespace Backend {

    class GraphicsBackend {
    public:
        GraphicsBackend(const std::string &window_title, uint32_t width, uint32_t height);

        inline Platform::Window &GetWindow() { return m_window; }
        inline VulkanBackend::VulkanRenderer &GetVulkanRenderer() { return m_renderer; }

    private:
        Platform::Window m_window;
        VulkanBackend::VulkanRenderer m_renderer;
    };

}
