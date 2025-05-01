#include "Renderer.h"
#include "glm/fwd.hpp"
#include <cstdlib>
#include <iostream>
#include <random>

namespace Renderer {

    Renderer::Renderer(uint32_t width, uint32_t height, VkFormat format, std::unique_ptr<Tracer> tracer)
        : m_film(width, height, format)
        , m_tracer(std::move(tracer))
    {
        m_film.Fill(Color(0.0f, 0.0f, 0.0f, 1.0f));

        random_indices.resize(width * height);
        std::iota(random_indices.begin(), random_indices.end(), 0u);

        static std::mt19937 rng { std::random_device{}() };
        std::shuffle(random_indices.begin(), random_indices.end(), rng);
    }

    Film& Renderer::RenderToFilm(Scene::Scene &scene) {
        const uint32_t total_pixel = static_cast<uint32_t>(random_indices.size());
        const uint32_t remaining = total_pixel - m_current_offset;
        const uint32_t samples_this_frame = std::min(SAMPLES_PER_FRAME, remaining);
        
        for (size_t k = 0; k < samples_this_frame; ++k) {
            uint32_t index = random_indices[m_current_offset + k];
            uint32_t x = index % m_film.Width();
            uint32_t y = index / m_film.Width();

            Geometry::Ray ray = scene.GetCamera().GenerateRay(x, y);
            Color result_color = m_tracer->Trace(scene, ray, MAX_RAY_DEPTH);
            m_film.PutColor(x, y, result_color);
        }
        m_current_offset += samples_this_frame;

        return m_film;
    }

}
