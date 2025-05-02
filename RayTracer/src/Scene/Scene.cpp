#include "Scene.h"
#include "Materials/Dielectric.h"

namespace Scene {

    Scene::Scene() {

    }

    Color Scene::DirectIllumination(const glm::vec3 &point, const glm::vec3 &normal) const {
        Color result { 0.0f };

        for (const auto &light : m_lights) {
            auto [shadow_ray, max_dist] = light->ComputeShadowRay(point);
            auto blocker = IntersectNearest(shadow_ray);

            if (!blocker || blocker->time > max_dist) {
                result += light->Illuminate(point, normal);
            } else {
                if (auto die = dynamic_cast<const Materials::Dielectric*>(blocker->material)) {
                    float eta = 1.0f / die->GetIndexOfRefraction();
                    float R = die->ComputeReflectance(shadow_ray.Direction(), blocker->normal, eta);
                    float T_beer = std::exp(-die->GetAbsorption() * blocker->time);
                    float T = (1.0f - R) * T_beer;
                    result += T * light->Illuminate(point, normal);
                }
            }
        }

        
        return glm::vec4(result.r, result.g, result.b, 1.0f);
    }

}
