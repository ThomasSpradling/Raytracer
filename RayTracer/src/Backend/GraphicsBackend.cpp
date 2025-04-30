#include "GraphicsBackend.h"
#include <iostream>

namespace Backend {
 
    GraphicsBackend::GraphicsBackend(const std::string &window_title, uint32_t width, uint32_t height)
        : m_window(window_title, width, height)
        , m_renderer(m_window)
    {}
    
}
