#pragma once

#include "Common/Color.h"
#include "Renderer/Tracer.h"
#include "Scene/Scene.h"

namespace Materials {

    class Dielectric : public Material {
    public:
        Dielectric(float index_of_refraction = 1.0f, float absorption = 0.0f, float diffuse_ratio = 0.0f, const Color &albedo = glm::vec4(1.0f));
        virtual Color Shade(const Geometry::Intersection &intersection, const Scene::Scene &scene, const Geometry::Ray &in_ray, const Renderer::Tracer &tracer, int depth) const override;
        inline void SetAbsorption(float absorption) { m_absorption = absorption; }

        float ComputeReflectance(const glm::vec3 &direction, const glm::vec3 &normal, float eta) const;

        inline float GetIndexOfRefraction() const { return m_index_of_refraction; }
        inline float GetAbsorption() const { return m_absorption; }
        inline float GetDiffuseRatio() const { return m_diffuse_ratio; }
    private:    
        float m_index_of_refraction; // ratio n_2 / n_1
        float m_absorption;
    
        Color m_albedo;
        float m_diffuse_ratio;
    };

}
