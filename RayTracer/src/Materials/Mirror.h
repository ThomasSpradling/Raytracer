#pragma once


#include "Common/Color.h"
#include "Geometry/Ray.h"
#include "Renderer/Tracer.h"
#include "Scene/Scene.h"

namespace Materials {

    class Mirror : public Material {
    public:
        Mirror(const Color &tint);
        virtual Color Shade(const Geometry::Intersection& intersection, const Scene::Scene &scene, const Geometry::Ray &in_ray, const Renderer::Tracer &tracer, int depth = 0) const;
    private:
        Color m_tint;
    };

}