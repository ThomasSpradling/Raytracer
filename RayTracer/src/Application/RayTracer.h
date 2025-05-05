#pragma once

#include "Backend/GraphicsBackend.h"
#include "Renderer/Renderer.h"
#include "Scene/Scene.h"

namespace Application {

    /**
     * @brief Handles the running of main loop and components.
     * It is to be handled by the user
     */
    class RayTracer {
    public:
        RayTracer(uint32_t video_width, uint32_t video_height, uint32_t samples_per_pixel);
        inline void SetOutputPath(const std::string &path) { m_output_path = path; }
        inline void SetNoGui(bool value) { m_no_gui = value; }

        void Run();
        void SetScene(std::shared_ptr<Scene::Scene> scene) { m_scene = scene; }
    private:
        std::unique_ptr<Renderer::Renderer> m_renderer;
        Backend::GraphicsBackend m_graphics_backend;
        std::shared_ptr<Scene::Scene> m_scene;
    private:
        void Update();
        std::string m_output_path = "";
        bool m_no_gui = false;
    };

}
