#include "Geometry/Sphere.h"
#include "Geometry/Intersections.h"
#include "Geometry/Primitive.h"
#include "glm/ext/scalar_constants.hpp"
#include <cmath>
#include <iostream>

namespace Geometry {

    Sphere::Sphere(const glm::vec3 &center, float radius, std::shared_ptr<Materials::Material> material) 
        : m_center(center)
        , m_radius(radius)
        , Primitive(material)
    {}

    std::optional<Intersection> Sphere::Intersect(const Ray &ray, float tmin, float tmax) const {
        glm::vec3 offset = ray.Origin() - m_center;

        float a = glm::dot(ray.Direction(), ray.Direction());
        float b = glm::dot(ray.Direction(), offset);
        float c = glm::dot(offset, offset) - m_radius * m_radius;

        float discriminant = b * b - a * c;
        if (discriminant < 0) {
            // No intersection
            return std::nullopt;
        }

        float sqrt_discriminant = std::sqrt(discriminant);

        float time = (-b - sqrt_discriminant) / a;
        if (time < tmin || time > tmax) {
            time = (-b + sqrt_discriminant) / a;
            if (time < tmin || time > tmax) {                
                return std::nullopt;
            }
        }

        glm::vec3 point = ray(time);
        glm::vec3 normal = glm::normalize(point - m_center);

        float phi = std::atan2(normal.z, normal.x);
        if (phi < 0) phi += 2 * glm::pi<float>();
            float theta = std::acos(glm::clamp(normal.y, -1.0f, 1.0f));

        glm::vec2 uv {
            phi / (2 * glm::pi<float>()),
            theta / glm::pi<float>()
        };

        return Intersection(ray, point, normal, uv, time, m_material.get());
    }

}
