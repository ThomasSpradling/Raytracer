#include "Scene.h"
#include "Materials/Dielectric.h"

namespace Scene {

    Scene::Scene() {

    }

    Color Scene::DirectIllumination(const glm::vec3 &point, const glm::vec3 &normal) const {
        PROFILE_FUNCTION_AUTO();

        Color result { 0.0f };

        for (const auto &light : m_lights) {
            auto [shadow_ray, max_dist] = light->ComputeShadowRay(point);
            auto blocker = IntersectNearest(shadow_ray);

            if (!blocker || blocker->Time() > max_dist) {
                result += light->Illuminate(point, normal);
            } else {
                if (auto die = dynamic_cast<const Materials::Dielectric *>(blocker->Material())) {
                    float eta = 1.0f / die->GetIndexOfRefraction();
                    float reflectance = die->ComputeReflectance(shadow_ray.Direction(), blocker->Normal(), eta);
                    float beer_falloff = std::exp(-die->GetAbsorption() * blocker->Time());
                    float transmittance = (1.0f - reflectance) * beer_falloff;
                    result += (die->GetDiffuseRatio() + (1.0f - die->GetDiffuseRatio()) * transmittance) * light->Illuminate(point, normal);
                }
            }
        }

        
        return glm::vec4(result.r, result.g, result.b, 1.0f);
    }

}
