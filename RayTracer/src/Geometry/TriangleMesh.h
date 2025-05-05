#pragma once

#include "Geometry/Primitive.h"
#include "Materials/Material.h"
#include <vector>
namespace Geometry {

    template <typename T>
    concept VectorLike = requires(T a, float f) {
        { f * a } -> std::convertible_to<T>;
        { a + a } -> std::convertible_to<T>;
    };

    class TriangleMesh : public Primitive {
    public:
        TriangleMesh(std::shared_ptr<Materials::Material> material);

        inline void SetVertices(const std::vector<glm::vec3> &vertices) { m_positions = vertices; }
        inline void SetUVs(const std::vector<glm::vec2> &uvs) { m_texture_coords = uvs; }
        inline void SetIndices(const std::vector<uint32_t> &indices) { m_indices = indices; }

        virtual std::optional<Intersection> Intersect(
            const Ray &ray,
            float tmin = 0,
            float tmax = std::numeric_limits<float>::infinity()) const override;

        template <VectorLike T>
        T Interpolate(float u, float v, T attribute0, T attribute1, T attribute2) const {
            return (1 - u - v) * attribute0 + u * attribute1 + v * attribute2;
        }
    private:
        std::vector<glm::vec3> m_positions;
        std::vector<glm::vec2> m_texture_coords;
        std::vector<uint32_t> m_indices;
    };

}
