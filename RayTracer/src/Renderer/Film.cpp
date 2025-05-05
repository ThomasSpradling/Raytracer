#include "Film.h"
#include "Common/Color.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace Renderer {

    Film::Film(uint32_t width, uint32_t height, VkFormat format, const Color &initial_color)
        : m_width{width}
        , m_height{height}
        , m_format(format)
        , m_data(width * height * CHANNEL_COUNT)
    {
        switch (format) {
            case VK_FORMAT_R8G8B8A8_UNORM: {
                m_layout = Layout::RGBA;
                m_is_linear_colorspace = true;
                break;
            }
            case VK_FORMAT_R8G8B8A8_SRGB: {
                m_layout = Layout::RGBA;
                m_is_linear_colorspace = false;
                break;
            }
            case VK_FORMAT_B8G8R8A8_UNORM: {
                m_layout = Layout::BGRA;
                m_is_linear_colorspace = true;
                break;
            }
            case VK_FORMAT_B8G8R8A8_SRGB: {
                m_layout = Layout::BGRA;
                m_is_linear_colorspace = false;
                break;
            }
            default:
                throw std::runtime_error("Unsupported swap chain VkFormat for Film");
        }
    }

    void Film::PutColor(uint32_t i, uint32_t j, const Color &color) {
        if (m_layout == Layout::RGBA) {
            m_data[Index(i, j, 0)] = Pack(color.r);
            m_data[Index(i, j, 1)] = Pack(color.g);
            m_data[Index(i, j, 2)] = Pack(color.b);
            m_data[Index(i, j, 3)] = Pack(color.a);
        } else if (m_layout == Layout::BGRA) {
            m_data[Index(i, j, 0)] = Pack(color.b);
            m_data[Index(i, j, 1)] = Pack(color.g);
            m_data[Index(i, j, 2)] = Pack(color.r);
            m_data[Index(i, j, 3)] = Pack(color.a);
        }
    }

    void Film::Fill(const Color &color) {
        std::array<uint8_t, 4> packed;
        if (m_layout == Layout::RGBA) {
            packed[0] = Pack(color.r);
            packed[1] = Pack(color.g);
            packed[2] = Pack(color.b);
        } else if (m_layout == Layout::BGRA) {
            packed[0] = Pack(color.b);
            packed[1] = Pack(color.g);
            packed[2] = Pack(color.r);
        }
        packed[3] = Pack(color.a);

        uint32_t pixel;
        std::memcpy(&pixel, packed.data(), 4);

        const size_t word_count = m_data.size() / 4;
        auto* dest = reinterpret_cast<uint32_t*>(m_data.data());
        std::fill_n(dest, word_count, pixel);
    }

    void Film::WriteToImage(const std::string &output_path) {
        std::cout << "writing to image" << std::endl;
        std::cout << "Dimensions: " << m_width << " x " << m_height << std::endl;

        std::vector<uint8_t> rgba;
        rgba.resize(m_width * m_height * CHANNEL_COUNT);

        if (m_layout == Layout::RGBA) {
            std::copy(m_data.begin(), m_data.end(), rgba.begin());
        } else {
            for (size_t i = 0; i < m_data.size(); i += CHANNEL_COUNT) {
                rgba[i + 0] = m_data[i + 2];
                rgba[i + 1] = m_data[i + 1];
                rgba[i + 2] = m_data[i + 0];
                rgba[i + 3] = m_data[i + 3];
            }
        }

        int stride = m_width * CHANNEL_COUNT;
        if (!stbi_write_png(
            output_path.c_str(),
            static_cast<int>(m_width),
            static_cast<int>(m_height),
            CHANNEL_COUNT,
            rgba.data(),
            stride
        )) {
            throw std::runtime_error("Film::WriteToImage: Failed to write to path, " + output_path);
        }
    }

    uint32_t Film::Index(uint32_t i, uint32_t j, uint32_t c) const {
        assert(0 <= i && i < m_width);
        assert(0 <= j && j < m_height);
        assert(0 <= c && c < CHANNEL_COUNT);
        return (j * m_width + i) * CHANNEL_COUNT + c;
    }

    uint8_t Film::Pack(float value) {
        if (m_is_linear_colorspace) {
            return Common::LinearColorToByte(value);
        } else {
            return Common::SRGBColorToByte(value);
        }
    }

}
