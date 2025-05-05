#pragma once

#include "Common/Color.h"
#include "Geometry/Ray.h"
#include "Scene/Light.h"
#include <glm/glm.hpp>

namespace Scene {

    class PointLight : public Light {
    public:
        PointLight(const glm::vec3 &position,
               const Color &intensity,
               float constant = 1.0f,
               float linear = 0.0f,
               float quadratic = 0.0f);
        virtual Color Illuminate(const glm::vec3 &point, const glm::vec3 &normal) const override;
        virtual std::pair<Geometry::Ray, float> ComputeShadowRay(const glm::vec3 &point) const override;
    private:
        glm::vec3 m_position {};
        float m_constant, m_linear, m_quadratic;
        Color m_intensity;
    };

}
