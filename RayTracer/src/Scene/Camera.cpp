#include "Camera.h"
#include "Utils/Profiler.h"
#include "glm/geometric.hpp"
#include <cmath>

namespace Scene {

    Camera::Camera(const glm::vec3 &look_from, const glm::vec3 &look_at, float field_of_view, const glm::vec3 &up, float focal_lengths)
        : m_center(look_from)
        , m_direction(glm::normalize(look_at - look_from))
        , m_field_of_view(field_of_view)
        , m_focal_length(focal_lengths)
        , m_up(up)
    {
        Update();
    }
    
    Geometry::Ray Camera::GenerateRay(float u, float v) {
        PROFILE_FUNCTION_AUTO();

        // For now, assume no multi-sampling

        glm::vec3 film_location = m_top_left_pixel
            + u * m_delta_u
            + v * m_delta_v;
        
        return {
            m_center,
            glm::normalize(film_location - m_center),
        };
    }

    void Camera::Update() {
        m_aspect = static_cast<float>(m_image_size.x) / static_cast<float>(m_image_size.y);

        m_sensor_height = 2 * m_focal_length * glm::tan(m_field_of_view / 2.0f);
        m_sensor_width = m_aspect * m_sensor_height;

        m_delta_u = glm::vec3(m_sensor_width / static_cast<float>(m_image_size.x), 0.0f, 0.0f);
        m_delta_v = glm::vec3(0.0f, m_sensor_height / static_cast<float>(m_image_size.y), 0.0f);
        m_top_left_pixel = m_center
            + glm::vec3(-m_sensor_width / 2.0f, m_sensor_height / 2.0f, -m_focal_length)
            + m_delta_u / 2.0f
            + m_delta_v / 2.0f;

        glm::vec3 w = glm::normalize(-m_direction);
        glm::vec3 u = glm::normalize(glm::cross(m_up, w));
        glm::vec3 v = glm::cross(w, u);

        m_aspect = static_cast<float>(m_image_size.x)
                / static_cast<float>(m_image_size.y);

        float fovRad = glm::radians(m_field_of_view);
        m_sensor_height = 2.0f * m_focal_length * glm::tan(fovRad * 0.5f);
        m_sensor_width  = m_sensor_height * m_aspect;

        m_delta_u =  u * (m_sensor_width  / static_cast<float>(m_image_size.x));
        m_delta_v = -v * (m_sensor_height / static_cast<float>(m_image_size.y));

        glm::vec3 film_center = m_center + m_direction * m_focal_length;
        m_top_left_pixel = film_center
            - 0.5f * m_sensor_width  * u
            + 0.5f * m_sensor_height * v
            + 0.5f * m_delta_u
            + 0.5f * m_delta_v;
    }

}
