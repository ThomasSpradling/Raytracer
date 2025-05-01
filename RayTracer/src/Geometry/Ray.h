#pragma once

#include <glm/glm.hpp>

namespace Geometry {
    
    class Ray {
    public:
        Ray(const glm::vec3 &origin, const glm::vec3 &direction);
        const glm::vec3 &Origin() const { return m_origin; }
        const glm::vec3 &Direction() const { return m_direction; }

        glm::vec3 operator()(float time) const;
    private:
        glm::vec3 m_origin;
        glm::vec3 m_direction;
    };

}