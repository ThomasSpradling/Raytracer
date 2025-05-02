#include "Renderer/WhittedTracer.h"
#include "Materials/Material.h"
#include <iostream>

namespace Renderer {
    
    Color WhittedTracer::Trace(const Scene::Scene &scene, const Geometry::Ray &ray, uint32_t depth) const {        
        if (const auto intersection = scene.IntersectNearest(ray); intersection.has_value()) {
            return intersection->material->Shade(*intersection, scene, ray, *this, depth);
        }

        return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    }

}
