#include "Renderer.h"
#include "Utils/Profiler.h"
#include "glm/fwd.hpp"
#include <cstdlib>
#include <iostream>
#include <random>

namespace Renderer {

    Renderer::Renderer(uint32_t width, uint32_t height, VkFormat format, uint32_t samples_per_pixel, std::unique_ptr<Tracer> tracer)
        : m_film(width, height, format)
        , m_tracer(std::move(tracer))
        , m_samples_per_pixel(samples_per_pixel)
    {
        m_film.Fill(Color(0.0f, 0.0f, 0.0f, 1.0f));

        random_indices.resize(width * height * m_samples_per_pixel);
        
        for (uint32_t i = 0; i < width * height; ++i)
            for (uint32_t s = 0; s < m_samples_per_pixel; ++s)
                random_indices[i * m_samples_per_pixel + s] = i;

        m_accum.assign(width * height, Color(0.0f));
        m_sample_count.assign(width * height, 0u);

        static std::mt19937 rng { std::random_device{}() };
        std::shuffle(random_indices.begin(), random_indices.end(), rng);
    }

    void _DrawProgressBar(uint32_t progress, uint32_t total) {
        float pct = float(progress) / float(total);
        const int barWidth = 50;
        int pos = int(pct * barWidth);

        std::cout << "\r[";
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos)      std::cout << '=';
            else if (i == pos) std::cout << '>';
            else              std::cout << ' ';
        }
        std::cout << "] " << int(pct * 100.0f) << "%  ("
                << progress << "/" << total << ")";

        std::cout.flush();
    }

    std::pair<Film &, bool> Renderer::RenderToFilm(Scene::Scene &scene) {
        PROFILE_FUNCTION_AUTO();
        
        const uint32_t total_pixel = static_cast<uint32_t>(random_indices.size());

        const uint32_t total_rays = static_cast<uint32_t>(random_indices.size());
        if (m_current_offset >= total_rays) {
            return { m_film, true };
        }

        const uint32_t remaining = total_rays - m_current_offset;
        const uint32_t samples_this_frame = std::min(SAMPLES_PER_FRAME, remaining);

        for (size_t k = 0; k < samples_this_frame; ++k) {
            uint32_t index = random_indices[m_current_offset + k];
            uint32_t x = index % m_film.Width();
            uint32_t y = index / m_film.Width();
            uint32_t px = y * m_film.Width() + x;

            static thread_local std::mt19937 gen{ std::random_device{}() };
            static thread_local std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            float ux = float(x) + dist(gen);
            float uy = float(y) + dist(gen);
            Geometry::Ray ray = scene.GetCamera().GenerateRay(ux, uy);
            Color result_color = m_tracer->Trace(scene, ray, MAX_RAY_DEPTH);
            // m_film.PutColor(x, y, result_color / float(m_samples_per_pixel));

            m_accum[px] += result_color;
            m_sample_count[px] += 1;

            Color avg = m_accum[px] / float(m_sample_count[px]);
            m_film.PutColor(x, y, avg);
        }
        m_current_offset += samples_this_frame;

        // --- draw progress bar ---
        _DrawProgressBar(m_current_offset, total_rays);

        return { m_film, false };
    }

}
