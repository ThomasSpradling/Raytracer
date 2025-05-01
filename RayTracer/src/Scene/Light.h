#pragma once

#include "Common/Color.h"
#include "Geometry/Ray.h"
#include <glm/glm.hpp>

namespace Scene {

    class Light {
    public:
        virtual ~Light() = default;
        virtual Color Illuminate(const glm::vec3 &point, const glm::vec3 &normal) const = 0;
        virtual std::pair<Geometry::Ray, float> ComputeShadowRay(const glm::vec3 &point) const = 0;
    };

}
