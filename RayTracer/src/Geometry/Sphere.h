#pragma once

#include "Geometry/Intersections.h"
#include "Geometry/Ray.h"
#include "Materials/Material.h"
#include "Primitive.h"
#include <glm/glm.hpp>
#include <optional>

namespace Geometry {
    
    class Sphere : public Primitive {
    public:
        Sphere(const glm::vec3 &center, float radius, std::shared_ptr<Materials::Material> material);
        virtual std::optional<Intersection> Intersect(
            const Ray &ray,
            float tmin = 0,
            float tmax = std::numeric_limits<float>::infinity()) const override;
    private:
        glm::vec3 m_center;
        double m_radius;
    };

}