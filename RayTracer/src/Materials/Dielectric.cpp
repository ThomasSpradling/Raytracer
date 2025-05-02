#include "Dielectric.h"
#include "Geometry/Ray.h"
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>         // core types
#include <glm/gtx/norm.hpp> 

namespace Materials {
    Dielectric::Dielectric(float index_of_refraction, float absorption, float diffuse_ratio, const Color &albedo)
        : m_index_of_refraction(index_of_refraction)
        , m_absorption(absorption)
        , m_diffuse_ratio(diffuse_ratio)
        , m_albedo(albedo)
    {}

    Color Dielectric::Shade(const Geometry::Intersection &intersection, const Scene::Scene &scene, const Geometry::Ray &in_ray, const Renderer::Tracer &tracer, int depth) const {
        if (depth == 0) return Color(0.0f, 0.0f, 0.0f, 1.0f);
        
        bool entering = intersection.front_face;
        float eta = entering ? 1.0f / m_index_of_refraction : m_index_of_refraction;
        glm::vec3 direction = glm::normalize(in_ray.Direction());

        float reflectance = ComputeReflectance(direction, intersection.normal, eta);
    
        Color result = 0.1f * m_albedo * scene.GetAmbientColor();
        result.a = 1.0f;

        if (m_diffuse_ratio > 0.0f) {
            Color diffuse_part = scene.DirectIllumination(intersection.point, intersection.normal);
            result += m_diffuse_ratio * m_albedo * diffuse_part;
        }

        if (m_diffuse_ratio < 1.0f) {
            float specular_scale = 1.0f - m_diffuse_ratio;
            
            Color reflected_component;
            if (entering) {
                glm::vec3 reflected_dir = glm::reflect(direction, intersection.normal);
                Geometry::Ray reflected_ray { intersection.point + 1e-4f * intersection.normal, reflected_dir };
                reflected_component = tracer.Trace(scene, reflected_ray, depth - 1);
            }

            glm::vec3 refracted_dir = glm::refract(direction, intersection.normal, eta);
            Color transmitted_component { 0.0f };
            if (glm::length2(refracted_dir) > 0.0f) {
                float optical_length = intersection.time;
                float beers_falloff = glm::exp(-m_absorption * optical_length);

                Geometry::Ray refracted_ray { intersection.point - 1e-4f * intersection.normal, refracted_dir };
                transmitted_component = tracer.Trace(scene, refracted_ray, depth - 1);
                transmitted_component *= beers_falloff;
            } else {
                // if refraction is impossible, then we reflect entirely
                reflectance = 1.0f;
            }

            result += specular_scale * glm::mix(transmitted_component, reflected_component, reflectance);
        }

        return result;
        
    }

    float Dielectric::ComputeReflectance(const glm::vec3 &direction, const glm::vec3 &normal, float eta) const {
        float cos_in = glm::clamp(dot(-direction, normal), 0.0f, 1.0f);
        
        float F0 = (1 - eta) / (1 + eta);
        F0 = F0 * F0;
        float reflectance = F0 + (1.0f - F0) * std::pow(1.0f - std::fabs(cos_in), 5.0f);
    }
}
