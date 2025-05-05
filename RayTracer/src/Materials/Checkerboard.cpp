#include "Checkerboard.h"
#include "Geometry/Intersections.h"
#include "Scene/Scene.h"
#include <iostream>

namespace Materials {

    Checkerboard::Checkerboard(const Color &color1, const Color &color2, float scale)
        : m_color1(color1)
        , m_color2(color2)
        , m_scale(scale)
    {}

    Color Checkerboard::Shade(const Geometry::Intersection &intersection, const Scene::Scene &scene, const Geometry::Ray &in_ray, const Renderer::Tracer &tracer, int) const {
        glm::vec2 uv = intersection.UV() * m_scale;
        int u = static_cast<int>(std::floor(uv.x));
        int v = static_cast<int>(std::floor(uv.y));
        Color base = (((u + v) & 1) == 0) ? m_color1 : m_color2;
        return base * (scene.GetAmbientColor() + scene.DirectIllumination(intersection.Point(), intersection.Normal()));
    }

}
