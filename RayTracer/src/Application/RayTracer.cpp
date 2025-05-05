#include "RayTracer.h"
#include "Renderer/Film.h"
#include "Renderer/WhittedTracer.h"
#include "Utils/Profiler.h"
#include <iostream>

namespace Application {

    RayTracer::RayTracer(uint32_t video_width, uint32_t video_height, uint32_t samples_per_pixel)
        : m_graphics_backend("Example Window", video_width, video_height)
    {
        VkExtent2D extent = m_graphics_backend.GetWindow().GetFramebufferExtent();
        VkFormat format = m_graphics_backend.GetVulkanRenderer().SwapChainFormat();

        m_renderer = std::make_unique<Renderer::Renderer>(extent.width, extent.height, format, samples_per_pixel, std::make_unique<Renderer::WhittedTracer>());
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
        
        auto [film, render_complete] = m_renderer->RenderToFilm(*m_scene);

        if (!m_no_gui) {
            m_graphics_backend.GetVulkanRenderer().Present(
                film.Data(),
                film.Width(),
                film.Height()
            );
        }

        if (render_complete && !m_output_path.empty()) {
            film.WriteToImage(m_output_path);
            std::cout << "Succesfully written to image: " << m_output_path << std::endl;
            m_output_path = "";
        }
    }

}
