#pragma once

#include <glm/glm.hpp>
#include "Materials/Material.h"

namespace Geometry {

    struct Intersection {
        glm::vec3 point;
        glm::vec3 normal;
        float time = 0;
        // medium
        const Materials::Material *material;
    };

}
