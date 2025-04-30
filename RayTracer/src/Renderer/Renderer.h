#pragma once

#include "Film.h"
#include <volk.h>

namespace Renderer {

    class Renderer {
    public:
        Renderer(uint32_t width, uint32_t height, VkFormat format);
        Film &RenderToFilm();
    private:
        Film m_film;
    };

}
