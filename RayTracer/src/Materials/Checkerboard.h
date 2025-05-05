#pragma once

#include "Common/Color.h"
#include "Renderer/Tracer.h"
#include "Scene/Scene.h"

namespace Materials {

    class Checkerboard : public Material {
    public:
        Checkerboard(const Color &color1 = glm::vec4(0,0,1,1), const Color &color2 = glm::vec4(1,0,0,1), float scale = 10.0f);
        virtual Color Shade(const Geometry::Intersection &intersection, const Scene::Scene &scene, const Geometry::Ray &in_ray, const Renderer::Tracer &tracer, int) const;
    private:
        Color m_color1;
        Color m_color2;
        float m_scale;
    };

}
