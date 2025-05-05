#include "Primitive.h"

namespace Geometry {
    Primitive::Primitive(std::shared_ptr<Materials::Material> material)
        : m_material(material)
    {}

    PrimitiveList::PrimitiveList() {}

    std::optional<Intersection> PrimitiveList::IntersectNearest(const Ray &ray, float tmin, float tmax) const {
        float min_time = std::numeric_limits<float>::infinity();
        std::optional<Intersection> result = std::nullopt;
        
        for (const auto &primitive : m_data) {
            if (auto intersection = primitive->Intersect(ray, tmin, tmax); intersection != std::nullopt) {
                if (intersection->Time() < min_time) {
                    min_time = intersection->Time();
                    result = intersection;
                }
            }
        }

        return result;
    }

    std::optional<Intersection> PrimitiveList::IntersectAny(const Ray &ray, float tmin, float tmax) const {
        for (const auto &primitive : m_data) {
            if (auto intersection = primitive->Intersect(ray, tmin, tmax); intersection != std::nullopt) {
                return intersection;
            }
        }

        return std::nullopt;
    }
}
