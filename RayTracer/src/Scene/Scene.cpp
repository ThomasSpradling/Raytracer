#include "Scene.h"

namespace Scene {

    Scene::Scene() {

    }

    Color Scene::DirectIllumination(const glm::vec3 &point, const glm::vec3 &normal) const {
        Color result { 0.3f, 0.3f, 0.3f, 1.0f };
        for (const auto &light : m_lights) {
            auto [shadow_ray, ray_length] = light->ComputeShadowRay(point);
            if (!IntersectAny(shadow_ray, 0.0f, ray_length)) {
                result += light->Illuminate(point, normal);
            }
        }
        return glm::vec4(result.r, result.g, result.b, 1.0f);
    }

}
