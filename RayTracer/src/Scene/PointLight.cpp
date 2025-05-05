#include "PointLight.h"
#include "Geometry/Ray.h"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"

namespace Scene {

    PointLight::PointLight(const glm::vec3 &position,
               const Color &intensity,
               float constant,
               float linear,
               float quadratic)
        : m_position(position)
        , m_intensity(intensity)
        , m_constant(constant)
        , m_linear(linear)
        , m_quadratic(quadratic)
    {}

    Color PointLight::Illuminate(const glm::vec3 &point, const glm::vec3 &normal) const {
        glm::vec3 to_light = m_position - point;
        float dist2 = glm::dot(to_light, to_light);
        float dist = std::sqrt(dist2);

        glm::vec3 direction = to_light / dist;
        float lambert_cosine = glm::max(0.0f, glm::dot(glm::normalize(direction), normal));

        // float attenuation = 1.0f / (m_constant
        //                     + m_linear * dist
        //                     + m_quadratic * dist2);
        float attenuation = 1.0f / (1.0f + dist2);
        
        float irradiance = lambert_cosine / glm::pi<float>();
        return m_intensity * attenuation * irradiance;
    }

    std::pair<Geometry::Ray, float> PointLight::ComputeShadowRay(const glm::vec3 &point) const {
        glm::vec3 to_light = m_position - point;
        float dist = glm::length(to_light);
        glm::vec3 direction = to_light / dist;

        float epsilon = 1e-4f;

        Geometry::Ray ray = { point + epsilon * direction, direction };
        return { ray, dist - epsilon };
    }
}
