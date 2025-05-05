#include "Geometry/TriangleMesh.h"
#include "Geometry/Intersections.h"
#include <optional>

namespace Geometry {

    TriangleMesh::TriangleMesh(std::shared_ptr<Materials::Material> material)
        : Primitive(material)
    {}

    std::optional<Intersection> TriangleMesh::Intersect(const Ray &ray, float tmin, float tmax) const {
        assert(m_indices.size() % 3 == 0);

        std::optional<Intersection> closest_hit;
        float best_time = tmax;

        for (uint32_t i = 0; i < m_indices.size(); i += 3) {
            uint32_t i0 = m_indices[i + 0];
            uint32_t i1 = m_indices[i + 1];
            uint32_t i2 = m_indices[i + 2];

            // Möller-Trumbore algorithm
            const glm::vec3 &v0 = m_positions[i0];
            const glm::vec3 &v1 = m_positions[i1];
            const glm::vec3 &v2 = m_positions[i2];

            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;

            glm::vec3 direction_cross_edge2 = glm::cross(ray.Direction(), edge2);
            float det = glm::dot(edge1, direction_cross_edge2);
            if (std::fabs(det) < 1e-8)
                continue;

            float inverse_det = 1.0f / det;
            glm::vec3 tvec = ray.Origin() - v0;
            float u = glm::dot(tvec, direction_cross_edge2) * inverse_det;
            if (u < 0.0f || u > 1.0f) 
                continue;

            glm::vec3 qvec = glm::cross(tvec, edge1);
            float v = glm::dot(ray.Direction(), qvec) * inverse_det;
            if (v < 0.0f || (u + v) > 1.0f) 
                continue;

            float t = glm::dot(edge2, qvec) * inverse_det;
            if (t < tmin || t > best_time) 
                continue;

            best_time = t;
            glm::vec3 hit_position = ray.Origin() + ray.Direction() * t;

            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
            if (glm::dot(normal, ray.Direction()) > 0.0f)
                normal = -normal;
            
            glm::vec2 uv { 0.0f };
            if (m_texture_coords.size() > 0) {
                assert(m_texture_coords.size() == m_positions.size());
                uv = Interpolate<glm::vec2>(u, v, m_texture_coords[i0], m_texture_coords[i1], m_texture_coords[i2]);
            }

            closest_hit = Intersection{
                ray,
                hit_position,
                normal,
                uv,
                t,
                m_material.get()
            };
        }

        return closest_hit;
    }
}
