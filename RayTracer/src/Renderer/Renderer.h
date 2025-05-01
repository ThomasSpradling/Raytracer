#pragma once

#include "Film.h"
#include "Geometry/Ray.h"
#include "Renderer/Tracer.h"
#include "Scene/Scene.h"
#include <volk.h>

namespace Renderer {

    class Renderer {
    public:
        Renderer(uint32_t width, uint32_t height, VkFormat format, std::unique_ptr<Tracer> tracer);
        Film &RenderToFilm(Scene::Scene &scene);
        Color Trace(Scene::Scene &scene, const Geometry::Ray &ray) const;
    private:
        Film m_film;

        static constexpr uint32_t MAX_RAY_DEPTH = 5;

        std::unique_ptr<Tracer> m_tracer;

        uint32_t m_current_offset = 0;
        static constexpr uint32_t SAMPLES_PER_FRAME = 500;
        std::vector<uint32_t> random_indices {};
    };

}
