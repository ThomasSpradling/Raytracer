#pragma once

#include "Camera.h"
#include "Light.h"
#include "Geometry/Intersections.h"
#include "Geometry/Ray.h"
#include "Geometry/Primitive.h"
#include "Scene/PointLight.h"
#include <vector>

namespace Scene {
    class Scene {
    public:
        Scene();
        inline Camera &GetCamera() { return *m_camera; }
        inline void SetCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }

        inline void Add(std::unique_ptr<Geometry::Primitive> primitive) { m_primitive_list.Add(std::move(primitive)); }
        
        template <class T, class... Args>
        void Add(Args &&...args) {
            static_assert(std::is_base_of_v<Geometry::Primitive, T>, "T must derive from Primitive");
            m_primitive_list.Add(std::make_unique<T>(std::forward<Args>(args)...));
        }

        inline std::optional<Geometry::Intersection> IntersectNearest(
            const Geometry::Ray &ray, 
            float tmin = 0,
            float tmax = std::numeric_limits<float>::infinity()) const { return m_primitive_list.IntersectNearest(ray, tmin, tmax); };

        inline std::optional<Geometry::Intersection> IntersectAny(
            const Geometry::Ray &ray, 
            float tmin = 0,
            float tmax = std::numeric_limits<float>::infinity()) const { return m_primitive_list.IntersectAny(ray, tmin, tmax); };

        template <class T, class... Args>
        void AddLight(Args &&...args) {
            static_assert(std::is_base_of_v<Light, T>, "T must derive from Light");
            m_lights.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        }

        Color DirectIllumination(const glm::vec3 &point, const glm::vec3 &normal) const;

        static Color GetAmbientColor() { return { 0.3f, 0.3f, 0.3f, 1.0f }; }
    private:
        std::shared_ptr<Camera> m_camera;
        Geometry::PrimitiveList m_primitive_list;
        std::vector<std::unique_ptr<Light>> m_lights;
    };
}
