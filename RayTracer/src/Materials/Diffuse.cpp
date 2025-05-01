#include "Diffuse.h"
#include "Geometry/Intersections.h"
#include "Scene/Scene.h"
#include <iostream>

namespace Materials {

    Diffuse::Diffuse(const Color &albedo)
        : m_albedo(albedo)
    {}

    Color Diffuse::Shade(const Geometry::Intersection &intersection, const Scene::Scene &scene, const Geometry::Ray &in_ray, const Renderer::Tracer &tracer, int) const {
        return m_albedo * scene.DirectIllumination(intersection.point, intersection.normal);
    }

}
