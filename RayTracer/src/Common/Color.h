#pragma once

#include <glm/glm.hpp>

using Color = glm::vec4;

namespace Common {

    inline uint8_t LinearColorToByte(float value) {
        value = glm::clamp(value, 0.0f, 1.0f);
        return static_cast<uint8_t>(value * 255.0f + 0.5f);
    }

    inline uint8_t SRGBColorToByte(float value) {
        value = glm::clamp(value,0.0f,1.0f);
        if (value <= 0.0031308f) value *= 12.92f;
        else value = 1.055f * std::pow(value, 1.0f/2.4f) - 0.055f;
        return static_cast<uint8_t>(value * 255.0f + 0.5f);
    }

}

