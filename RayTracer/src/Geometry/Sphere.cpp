#include "Geometry/Sphere.h"
#include "Geometry/Intersections.h"
#include <cmath>
#include <iostream>

namespace Geometry {

    Sphere::Sphere(const glm::vec3 &center, float radius, const Materials::Material *material) 
        : m_center(center)
        , m_radius(radius)
        , m_material(material)
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

        return Intersection{
            .time = time,
            .normal = normal,
            .point = point,
            .material = m_material,
        };
    }

}
