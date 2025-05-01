#pragma once

#include "Common/Color.h"
#include "Scene/Scene.h"
namespace Renderer {

    class Tracer {
    public:
        virtual ~Tracer() = default;
        virtual Color Trace(const Scene::Scene &scene, const Geometry::Ray &ray, uint32_t depth) const = 0;
    private:
    };
    
}
