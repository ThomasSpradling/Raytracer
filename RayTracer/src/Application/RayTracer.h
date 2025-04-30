#pragma once

#include "Backend/GraphicsBackend.h"
#include "Platform/Window.h"
#include "Renderer/Renderer.h"
#include "Backend/VulkanRenderer.h"

namespace Application {

    /**
     * @brief Handles the running of main loop and components.
     * It is to be handled by the user
     */
    class RayTracer {
    public:
        RayTracer(uint32_t video_width, uint32_t video_height);
        void Run();
    private:
        std::unique_ptr<Renderer::Renderer> m_renderer;
        Backend::GraphicsBackend m_graphics_backend;
    private:
        void Update();
    };

}
