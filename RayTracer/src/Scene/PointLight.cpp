#include "PointLight.h"
#include "Geometry/Ray.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"

namespace Scene {

    PointLight::PointLight(const glm::vec3 &position, float attenuation)
        : m_position(position)
        , m_attenuation(attenuation)
    {}

    Color PointLight::Illuminate(const glm::vec3 &point, const glm::vec3 &normal) const {
        Color intensity { 1.0f };

        glm::vec3 direction = m_position - point;

        float effective_incident = glm::max(0.0f, glm::dot(glm::normalize(direction), normal));
        float falloff  = 1.0f / (1.0f + 0.001f * glm::dot(direction, direction));

        return intensity * effective_incident * falloff;
    }

    std::pair<Geometry::Ray, float> PointLight::ComputeShadowRay(const glm::vec3 &point) const {
        glm::vec3 direction = m_position - point;
        float len = glm::length(direction);
        direction = direction / len;

        float epsilon = 1e-4;

        Geometry::Ray ray = { point + epsilon * direction, direction };
        return { ray, len - epsilon };
    }
}
