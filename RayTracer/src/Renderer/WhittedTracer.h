#pragma once

#include "Geometry/Ray.h"
#include "Renderer/Tracer.h"
namespace Renderer {

    class WhittedTracer : public Tracer {
    public:
        WhittedTracer() {}
        virtual Color Trace(const Scene::Scene &scene, const Geometry::Ray &ray, uint32_t depth) const override;
    private:
    };

}
