#include "Renderer.h"
#include <iostream>
#include <random>

namespace Renderer {

    Renderer::Renderer(uint32_t width, uint32_t height, VkFormat format)
        : m_film(width, height, format)
    {
        m_film.Fill(Color(1.0f, 0.0f, 0.0f, 1.0f));
    }

    Film& Renderer::RenderToFilm() {
        static std::mt19937                 rng{ std::random_device{}() };
        std::uniform_int_distribution<uint32_t> xpos(0, m_film.Width()  - 1);
        std::uniform_int_distribution<uint32_t> ypos(0, m_film.Height() - 1);
        std::uniform_real_distribution<float>   chan(0.f, 1.f);   // colour channel

        // m_film.Fill(Color(0.f, 0.f, 0.f, 1.f));                   // clear

        for (int i = 0; i < 10000; ++i)
        {
            uint32_t x = xpos(rng);
            uint32_t y = ypos(rng);

            Color c{ chan(rng), chan(rng), chan(rng), 1.f };      // random RGB
            m_film.PutColor(x, y, c);
        }

        return m_film;
    }

}
