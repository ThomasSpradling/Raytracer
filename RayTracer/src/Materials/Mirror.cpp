#include "Materials/Mirror.h"
#include "Geometry/Ray.h"
#include "glm/geometric.hpp"

namespace Materials {
    Mirror::Mirror(const Color &tint)
        : m_tint(tint)
    {}

    Color Mirror::Shade(const Geometry::Intersection &intersection, const Scene::Scene &scene, const Geometry::Ray &in_ray, const Renderer::Tracer &tracer, int depth) const {
        if (depth == 0) return m_tint;

        glm::vec3 reflected_dir = glm::normalize(glm::reflect(in_ray.Direction(), intersection.normal));
        Geometry::Ray reflected_ray { intersection.point + 1e-4f * intersection.normal, reflected_dir };
        return m_tint * tracer.Trace(scene, reflected_ray, depth - 1);
    }

}