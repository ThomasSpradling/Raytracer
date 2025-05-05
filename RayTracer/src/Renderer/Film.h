#pragma once

#include <vector>
#include <volk.h>
#include "Common/Color.h"

namespace Renderer {

    class Film {
    public:
        Film(uint32_t width, uint32_t height, VkFormat format, const Color &initial_color = glm::vec4{ 0.0f });
        std::vector<uint8_t> &Data() { return m_data; }
        uint32_t Width() const { return m_width; }
        uint32_t Height() const { return m_height; }

        void PutColor(uint32_t i, uint32_t j, const Color &color);
        void Fill(const Color &color);

        void WriteToImage(const std::string &output_path);
        
    private:
        enum class Layout { RGBA, BGRA };
        constexpr static uint32_t CHANNEL_COUNT = 4;

        std::vector<Color> m_accum;
        std::vector<uint8_t> m_data {};
        VkFormat m_format;
        uint32_t m_width {};
        uint32_t m_height {};

        Layout m_layout = Layout::RGBA;
        bool m_is_linear_colorspace = true;
    private:
        uint32_t Index(uint32_t i, uint32_t j, uint32_t c = 0) const;
        uint8_t Pack(float value);
    };

}
