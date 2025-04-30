#pragma once

namespace Geometry {
    
    class Ray {
    public:
        Ray(glm::vec3 origin, glm::vec3 direction);
    private:
        glm::vec3 m_origin;
        glm::vec3 m_direction;
    };

}