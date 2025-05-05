#pragma once

#include <glm/glm.hpp>
#include "Geometry/Ray.h"
#include "Materials/Material.h"

namespace Geometry {

    class Intersection {
    public:
        Intersection(const Ray &ray, glm::vec3 point, glm::vec3 normal, glm::vec2 uv, float time, const Materials::Material *material)
            : m_material(material)
            , m_time(time)
            , m_point(point)
            , m_uv(uv)
        {
            m_normal = glm::faceforward(normal, ray.Direction(), normal);
            m_front_face = (glm::dot(ray.Direction(), normal) < 0.0f);
        }

        inline const glm::vec3 &Point() const { return m_point; }
        inline float Time() const { return m_time; }
        inline const glm::vec3 &Normal() const { return m_normal; }
        inline const glm::vec2 &UV() const { return m_uv; }
        inline bool IsFrontFace() const { return m_front_face; }
        inline const Materials::Material *Material() const { return m_material; }
    private:
        glm::vec3 m_point {};
        float m_time = 0.0f;

        glm::vec3 m_normal {};
        glm::vec2 m_uv {};
        // medium
        
        bool m_front_face = false;
        const Materials::Material *m_material;
    };

}
