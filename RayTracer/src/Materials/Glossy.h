#pragma once

#include "Common/Color.h"
#include "Renderer/Tracer.h"
#include "Scene/Scene.h"

namespace Materials {

    class Glossy : public Material {
    public:
        Glossy(const Color &albedo, float specularity = 1.0f, const glm::vec3 &tint = glm::vec3(1.0f));
        virtual Color Shade(const Geometry::Intersection &intersection, const Scene::Scene &scene, const Geometry::Ray &in_ray, const Renderer::Tracer &tracer, int depth) const;
    private:
        Color m_albedo;
        float m_specularity;
        glm::vec3 m_tint;
    };

}
