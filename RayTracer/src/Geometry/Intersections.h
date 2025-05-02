#pragma once

#include <glm/glm.hpp>
#include "Geometry/Ray.h"
#include "Materials/Material.h"

namespace Geometry {

    struct Intersection {
        Intersection(const Ray &ray, glm::vec3 point, glm::vec3 normal, float time, const Materials::Material *material)
            : material(material)
            , time(time)
            , point(point)
            , front_face(glm::dot(ray.Direction(), normal) < 0)
            , normal(front_face ? normal : -normal)
        {}

        glm::vec3 point;
        bool front_face = false;
        glm::vec3 normal;
        float time = 0;
        // medium
        const Materials::Material *material;
    };

}
