
#include "Platform/Window.h"
#include <iostream>
#include <stdexcept>

namespace Platform {

    Window::Window(const std::string &title, uint32_t width, uint32_t height) {
        std::cout << "Setting up window." << std::endl;

        if (!glfwInit()) {
            throw std::runtime_error("Error initializing GLFW!");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!m_handle) {
            throw std::runtime_error("Error creating GLFW window!");
        }
    }

    Window::~Window() {
        std::cout << "Destroying window." << std::endl;

        glfwDestroyWindow(m_handle);
        glfwTerminate();
    }

    VkExtent2D Window::GetWindowExtent() const {
        uint32_t width, height;
        glfwGetWindowSize(m_handle,
            reinterpret_cast<int*>(&width),
            reinterpret_cast<int*>(&height));

        return { width, height };
    }

    VkExtent2D Window::GetFramebufferExtent() const {
        uint32_t width, height;
        glfwGetFramebufferSize(m_handle,
            reinterpret_cast<int*>(&width),
            reinterpret_cast<int*>(&height));

        return { width, height };
    }

}

