
#include "Geometry/Ray.h"
namespace Geometry {

    Ray::Ray(const glm::vec3 &origin, const glm::vec3 &direction)
        : m_origin(origin)
        , m_direction(direction)
    {}

    glm::vec3 Ray::operator()(float time) const {
        return m_direction * time + m_origin;
    }

}
