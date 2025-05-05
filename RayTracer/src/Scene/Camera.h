#pragma once

#include "Geometry/Ray.h"
#include <volk.h>
#include <glm/glm.hpp>

namespace Scene {

    class Camera {
    public:
        Camera(
            const glm::vec3 &look_from = glm::vec3(0.0f),
            const glm::vec3 &look_at = glm::vec3(0.0f, 0.0f, -1.0f),
            float field_of_view = 45.0f,
            const glm::vec3 &up = { 0, 1, 0 },
            float focal_length = 1.0f
        );
        void SetImageSize(uint32_t width, uint32_t height) { m_image_size = { width, height }; }
        Geometry::Ray GenerateRay(float u, float v);
        void Update();
    private:
        glm::ivec2 m_image_size { 0, 0 };

        glm::vec3 m_center;
        glm::vec3 m_direction;
        float m_field_of_view;
        float m_focal_length;

        float m_aspect;

        float m_sensor_height = 0.0f;
        float m_sensor_width = 0.0f;

        glm::vec3 m_up = glm::vec3(0.0f);

        glm::vec3 m_delta_u = glm::vec3(0.0f);
        glm::vec3 m_delta_v = glm::vec3(0.0f);
        glm::vec3 m_top_left_pixel = glm::vec3(0.0f);
    };

}
