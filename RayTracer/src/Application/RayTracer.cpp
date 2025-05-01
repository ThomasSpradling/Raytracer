#include "RayTracer.h"
#include "Renderer/Film.h"
#include "Renderer/WhittedTracer.h"
#include "Utils/Profiler.h"

namespace Application {

    RayTracer::RayTracer(uint32_t video_width, uint32_t video_height)
        : m_graphics_backend("Example Window", video_width, video_height)
    {
        VkExtent2D extent = m_graphics_backend.GetWindow().GetFramebufferExtent();
        VkFormat format = m_graphics_backend.GetVulkanRenderer().SwapChainFormat();

        m_renderer = std::make_unique<Renderer::Renderer>(extent.width, extent.height, format, std::make_unique<Renderer::WhittedTracer>());
    }

    void RayTracer::Run() {
        while (!m_graphics_backend.GetWindow().ShouldClose()) {
            m_graphics_backend.GetWindow().PollEvents();
            Update();
        }

        Utils::Profiler::LogSummary();
    }
    
    void RayTracer::Update() {
        auto [width, height] = m_graphics_backend.GetWindow().GetFramebufferExtent();
        m_scene->GetCamera().SetImageSize(width, height);
        m_scene->GetCamera().Update();

        Renderer::Film &film = m_renderer->RenderToFilm(*m_scene);

        m_graphics_backend.GetVulkanRenderer().Present(
            film.Data(),
            film.Width(),
            film.Height()
        );
    }

}
