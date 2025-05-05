#include "Glossy.h"
#include "glm/ext/quaternion_common.hpp"

namespace Materials {

    Glossy::Glossy(const Color &albedo, float specularity, const Color &tint)
        : m_albedo(albedo)
        , m_specularity(glm::clamp(specularity, 0.0f, 1.0f))
        , m_tint(tint)
    {}

    Color Glossy::Shade(const Geometry::Intersection &intersection, const Scene::Scene &scene, const Geometry::Ray &in_ray, const Renderer::Tracer &tracer, int depth) const {
        Color diffuse_component = m_albedo * (scene.GetAmbientColor() + scene.DirectIllumination(intersection.Point(), intersection.Normal()));
        
        if (depth == 0 || m_specularity == 0.0f) return diffuse_component;
                
        constexpr float epsilon = 1e-4f;

        glm::vec3 reflected_dir = glm::normalize(glm::reflect(in_ray.Direction(), intersection.Normal()));
        Geometry::Ray reflected_ray { intersection.Point() + 1e-4f * intersection.Normal(), reflected_dir };
        Color specular_component = m_tint * tracer.Trace(scene, reflected_ray, depth - 1);

        return glm::mix(diffuse_component, specular_component, m_specularity);
    }

}
