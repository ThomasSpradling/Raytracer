#pragma once

#include "Common/Color.h"
#include "Geometry/Ray.h"
#include "Scene/Light.h"
#include <glm/glm.hpp>

namespace Scene {

    class PointLight : public Light {
    public:
        PointLight(const glm::vec3 &position, float attenuation = 1.0f);
        virtual Color Illuminate(const glm::vec3 &point, const glm::vec3 &normal) const override;
        virtual std::pair<Geometry::Ray, float> ComputeShadowRay(const glm::vec3 &point) const override;
    private:
        glm::vec3 m_position {};
        float m_attenuation;
    };

}
