#pragma once

#include "Common/Color.h"
#include "Renderer/Tracer.h"
#include "Scene/Scene.h"

namespace Materials {

    class Diffuse : public Material {
    public:
        Diffuse(const Color &albedo);
        virtual Color Shade(const Geometry::Intersection& intersection, const Scene::Scene &scene, const Geometry::Ray &in_ray, const Renderer::Tracer &tracer, int) const;
    private:
        Color m_albedo;
    };

}
