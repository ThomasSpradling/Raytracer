#pragma once

#include "Geometry/Intersections.h"
#include "Geometry/Ray.h"
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>
namespace Geometry {

    class Primitive {
    public:
        virtual ~Primitive() = default;
        virtual std::optional<Intersection> Intersect(const Ray &ray, float tmin, float tmax) const = 0;
    };

    class PrimitiveList {
    public:
        PrimitiveList();

        inline void Add(std::unique_ptr<Primitive> primitive) { m_data.push_back(std::move(primitive)); }
        
        template <class T, class... Args>
        void Add(Args &&...args) {
            static_assert(std::is_base_of_v<Primitive, T>, "T must derive from Primitive");
            m_data.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        }

        std::optional<Intersection> IntersectNearest(
            const Ray &ray, 
            float tmin = 0,
            float tmax = std::numeric_limits<float>::infinity()) const;

        std::optional<Intersection> IntersectAny(
            const Ray &ray, 
            float tmin = 0,
            float tmax = std::numeric_limits<float>::infinity()) const;
    private:
        std::vector<std::unique_ptr<Primitive>> m_data;
    };

}
