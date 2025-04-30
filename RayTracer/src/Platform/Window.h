#pragma once

#include <volk.h>
#include <GLFW/glfw3.h>
#include <string>

namespace Platform {
    
    class Window {
    public:
        Window(const std::string &title, uint32_t width, uint32_t height);
        ~Window();

        void PollEvents() const { glfwPollEvents(); }
        inline bool ShouldClose() const { return glfwWindowShouldClose(m_handle); }
        inline GLFWwindow *GetHandle() const { return m_handle; }

        VkExtent2D GetWindowExtent() const;
        VkExtent2D GetFramebufferExtent() const;
    private:
        GLFWwindow *m_handle;
    };

}
