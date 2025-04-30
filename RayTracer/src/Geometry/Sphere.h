#pragma once

namespace Geometry {
    
    class Sphere {
    public:
        Sphere(glm::vec3 center, double radius);
    private:
        glm::vec3 m_center;
        double m_radius;
    };

}