#pragma once

#include "Common/Color.h"
#include "Geometry/Ray.h"

namespace Geometry { struct Intersection; }
namespace Renderer { class Tracer; }
namespace Scene { class Scene; }

namespace Materials {

    class Material {
    public:
        virtual ~Material() = default;

        virtual Color Shade(const Geometry::Intersection &intersection, const Scene::Scene &scene, const Geometry::Ray &in_ray, const Renderer::Tracer &tracer, int depth = 0) const = 0;
    };

}
